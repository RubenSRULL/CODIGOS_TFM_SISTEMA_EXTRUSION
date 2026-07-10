# Autor: Rubén Sahuquillo Redondo
# Descripción: Clase de control de la unidad de control (UC) que maneja la comunicación UART y procesa la telemetría y los estados de la máquina.

# Funcionamiento:
# 1. La clase ControlUC se inicializa con una instancia de UART_COM para la comunicación con la unidad de control.
# 2. Se mantiene un diccionario de telemetría que almacena los valores actuales de estado, temperatura, objetivo, calefactor, RPMs, diámetro y alarmas.
# 3. Se procesan las líneas recibidas desde la unidad de control, actualizando la telemetría y manejando las respuestas pendientes.
# 4. Se proporcionan métodos para enviar comandos a la unidad de control, esperar respuestas y consultar el estado actual. 

# ==========================
# ===== Importaciones ======
# ==========================
import time

# ==========================================================
# ===== Estados reales generados por Simulink - Extrusora ===
# ==========================================================
#   0 BOOT
#   1 STOP
#   2 IDLE_AUTO
#   3 PREHEAT
#   4 EXTRUDING
#   5 MANUAL
#   6 ALARM

MACHINE_BOOT = 0
MACHINE_STOP = 1
MACHINE_IDLE_AUTO = 2
MACHINE_PREHEAT = 3
MACHINE_EXTRUDING = 4
MACHINE_MANUAL = 5
MACHINE_ALARM = 6

# Alias para mantener compatible el main.py existente.
# OJO: Simulink no tiene AUTO_READY ni STOPPING separados.
MACHINE_IDLE = MACHINE_STOP
MACHINE_AUTO_INIT = MACHINE_IDLE_AUTO
MACHINE_AUTO_PREHEAT = MACHINE_PREHEAT
MACHINE_AUTO_READY = MACHINE_PREHEAT
MACHINE_AUTO_EXTRUDING = MACHINE_EXTRUDING
MACHINE_MANUAL_CONTROL = MACHINE_MANUAL
MACHINE_STOPPING = MACHINE_STOP

# ==========================================================
# ===== Estados reales generados por Simulink - Enrolladora =
# ==========================================================
#   0 BOOT
#   1 STOP
#   2 HOME
#   3 RUNNING
#   4 ALARM

WINDER_BOOT = 0
WINDER_STOP = 1
WINDER_HOME = 2
WINDER_RUNNING = 3
WINDER_ALARM = 4

# Alias para mantener compatible el main.py existente.
WINDER_OFF = WINDER_BOOT
WINDER_IDLE = WINDER_STOP
WINDER_READY = WINDER_HOME
WINDER_STOPPING = WINDER_STOP

# ==============================================
# ===== Diccionarios de Nombres de Estados =====
# ==============================================
NOMBRES_ESTADOS_STM32 = {
    MACHINE_BOOT: "BOOT",
    MACHINE_STOP: "STOP",
    MACHINE_IDLE_AUTO: "IDLE_AUTO",
    MACHINE_PREHEAT: "PREHEAT",
    MACHINE_EXTRUDING: "EXTRUDING",
    MACHINE_MANUAL: "MANUAL",
    MACHINE_ALARM: "ALARM",
}

NOMBRES_ESTADOS_WINDER = {
    WINDER_BOOT: "BOOT",
    WINDER_STOP: "STOP",
    WINDER_HOME: "HOME",
    WINDER_RUNNING: "RUNNING",
    WINDER_ALARM: "ALARM",
}

# ============================================
# ===== Estados de la Sub FSM Automática =====
# ============================================
AUTO_STATES = {
    MACHINE_IDLE_AUTO,
    MACHINE_PREHEAT,
    MACHINE_EXTRUDING,
}

# =============================================
# ===== Estados de la Sub FSM Parada/Stop =====
# =============================================
STOPPABLE_STATES = {
    MACHINE_PREHEAT,
    MACHINE_EXTRUDING,
    MACHINE_MANUAL,
}

# ============================================
# ===== Fases de la FSM Automática (UI) =====
# ============================================
FASES_AUTOMATICAS = [
    {"titulo": "Configuración", "descripcion": "Configura material, diámetro objetivo y tiempo de proceso."},
    {"titulo": "Precalentamiento", "descripcion": "Envía SET_TEMP y PREHEAT; la FSM queda en PREHEAT."},
    {"titulo": "Listo", "descripcion": "Si la FSM está en PREHEAT y el firmware permite extrusión, pulsa START."},
    {"titulo": "Extrusión", "descripcion": "Proceso automático en marcha."},
    {"titulo": "Parada", "descripcion": "Detención ordenada y retorno a STOP."},
]

# =========================================
# ===== Clase de Control de la Unidad =====
# =========================================
class ControlUC:
    def __init__(self, uart):
        """
        Descripción: Inicializa la clase ControlUC con una instancia de UART_COM. Configura la telemetría inicial y prepara la lista de respuestas pendientes.
        Parámetros:
            uart (UART_COM): Instancia de la clase UART_COM para la comunicación con la unidad de control.
        Retorna:
            None
        """
        self.uart = uart
        self.respuestas_pendientes = []
        self.telemetria = {
            "state": MACHINE_BOOT,
            "temp": 0.0,
            "target": 0.0,
            "heater": 0.0,
            "rpm_int": 0.0,
            "rpm_ext": 0.0,
            "diam": 0.0,
            "alarm": "0x00000000",
            "winder_state": WINDER_OFF,
            "winder_alarm": 0,
        }

    # -- Verifica si la comunicación con la unidad de control está disponible --
    def disponible(self):
        """
        Descripción: Verifica si la comunicación con la unidad de control está disponible. Esto se determina si la clase UART_COM está en modo simulado o si el puerto serial está abierto.
        Parámetros:
            None
        Retorna:
            bool: True si la comunicación está disponible, False en caso contrario.
        """
        return self.uart.modo_simulado or self.uart.serial_port is not None

    # -- Lee mensajes pendientes de la cola UART y los procesa --
    def leer_pendiente(self):
        """
        Descripción: Lee mensajes pendientes de la cola UART y los procesa. Si hay mensajes disponibles, se procesan hasta que no queden más. Esto permite actualizar la telemetría y manejar las respuestas de la unidad de control.
        Parámetros:
            None
        Retorna:
            None
        """
        while True:
            linea = self.uart.get_mensaje(timeout=0.001)
            if linea is None:
                break
            self.procesar_linea(linea)

    # -- Procesa una línea recibida desde la unidad de control --
    def procesar_linea(self, linea):
        """
        Procesa una línea recibida desde la unidad de control.

        Importante: también actualiza la telemetría cuando la línea es una
        respuesta tipo OK:STATE=...;WINDER_STATE=... . Antes solo se guardaba
        como respuesta pendiente, por lo que los displays y SVG seguían usando
        el estado antiguo guardado en self.telemetria.
        """
        if not linea:
            return

        linea = linea.strip()

        if linea.startswith("TEL:"):
            self._procesar_telemetria(linea[4:])
            return

        if linea.startswith("OK:"):
            self._procesar_telemetria(linea[3:])
            self.respuestas_pendientes.append(linea)
            return

        if linea.startswith("OK") or linea.startswith("ERR"):
            self.respuestas_pendientes.append(linea)
            return

        if linea.startswith("TEMP:"):
            try:
                self.telemetria["temp"] = float(linea.split(":", 1)[1].split()[0])
            except ValueError:
                pass
            return

        if linea.startswith("WINDER:"):
            # Por si tienes debug directo de enrolladora:
            self._procesar_telemetria(linea.replace("WINDER:", "WINDER_"))
            return

    def _campos_desde_texto(self, texto):
        """
        Descripción: Extrae los campos clave-valor de un texto recibido desde la unidad de control. El texto puede comenzar con "TEL:", "OK:" o "ERR:", y los campos se separan por punto y coma. Cada campo tiene el formato "clave=valor".
        Parámetros:
            texto (str): Texto recibido desde la unidad de control.
        Retorna:
            dict: Diccionario con los campos extraídos, donde las claves son las claves en mayúsculas y los valores son los valores correspondientes.
        """
        if not texto:
            return {}

        texto = texto.strip()
        if texto.startswith("TEL:"):
            texto = texto[4:]
        elif texto.startswith("OK:"):
            texto = texto[3:]
        elif texto.startswith("ERR:"):
            texto = texto[4:]

        campos = {}
        for parte in texto.split(";"):
            if "=" in parte:
                clave, valor = parte.split("=", 1)
                campos[clave.strip().upper()] = valor.strip()
        return campos

    # --- Actualiza la telemetría desde un texto recibido ---
    def _actualizar_telemetria_desde_texto(self, texto):
        """
        Descripción: Actualiza la telemetría desde un texto recibido desde la unidad de control. Extrae los campos clave-valor y actualiza el diccionario de telemetría correspondiente.
        Parámetros:
            texto (str): Texto recibido desde la unidad de control.
        Retorna:
            None
        """
        campos = self._campos_desde_texto(texto)
        if not campos:
            return

        valores_float = {
            "TEMP": "temp",
            "TARGET": "target",
            "HEATER": "heater",
            "RPM_INT": "rpm_int",
            "RPM_EXT": "rpm_ext",
            "DIAM": "diam",
        }

        try:
            if "STATE" in campos:
                self.telemetria["state"] = int(campos["STATE"])

            if "WINDER_STATE" in campos:
                self.telemetria["winder_state"] = int(campos["WINDER_STATE"])

            if "WINDER_ALARM" in campos:
                self.telemetria["winder_alarm"] = int(campos["WINDER_ALARM"])

            for clave_uart, clave_local in valores_float.items():
                if clave_uart in campos:
                    self.telemetria[clave_local] = float(campos[clave_uart])

            if "ALARM" in campos:
                self.telemetria["alarm"] = campos["ALARM"]

        except ValueError:
            pass

    # --- Actualiza el estado estimado de la telemetría según el comando enviado --
    def _actualizar_estado_estimado_por_comando(self, comando):
        """
        Descripción: Actualiza el estado estimado de la telemetría según el comando enviado a la unidad de control. Esto permite que los displays y SVG reflejen el estado esperado inmediatamente después de enviar un comando, incluso antes de recibir una respuesta.
        Parámetros:
            comando (str): Comando enviado a la unidad de control.
        Retorna:
            None
        """
        comando = (comando or "").strip()

        cambios_extrusora = {
            "AUTO": MACHINE_IDLE_AUTO,
            "RESET_AUTO": MACHINE_STOP,
            "MANUAL": MACHINE_MANUAL,
            "PREHEAT": MACHINE_PREHEAT,
            "START": MACHINE_EXTRUDING,
            "STOP": MACHINE_STOP,
        }

        if comando in cambios_extrusora:
            self.telemetria["state"] = cambios_extrusora[comando]

        if comando == "START":
            self.telemetria["winder_state"] = WINDER_RUNNING
        elif comando in {"STOP", "STOP_EXT"}:
            self.telemetria["winder_state"] = WINDER_STOP
        elif comando.startswith("SET_EXT_RPM:"):
            self.telemetria["winder_state"] = WINDER_RUNNING

    # -- Procesa la telemetría recibida desde la unidad de control --
    def _procesar_telemetria(self, texto):
        """
        Descripción: Procesa la telemetría recibida desde la unidad de control. Extrae los valores de estado, temperatura, objetivo, calefactor, RPMs, diámetro y alarmas, y actualiza el diccionario de telemetría correspondiente.
        Parámetros:
            texto (str): Línea de telemetría recibida desde la unidad de control.
        Retorna:
            None
        """
        campos = {}

        for parte in texto.split(";"):
            if "=" in parte:
                clave, valor = parte.split("=", 1)
                campos[clave.strip().upper()] = valor.strip()

        valores_float = {
            "TEMP": "temp",
            "TARGET": "target",
            "HEATER": "heater",
            "RPM_INT": "rpm_int",
            "RPM_EXT": "rpm_ext",
            "DIAM": "diam",
        }

        try:
            if "EXTRUDER_STATE" in campos:
                self.telemetria["state"] = int(campos["EXTRUDER_STATE"])
            elif "STATE" in campos:
                self.telemetria["state"] = int(campos["STATE"])

            if "WINDER_STATE" in campos:
                self.telemetria["winder_state"] = int(campos["WINDER_STATE"])

            if "WINDER_ALARM" in campos:
                self.telemetria["winder_alarm"] = int(campos["WINDER_ALARM"])

            if "EXTRUDER_ALARM" in campos:
                self.telemetria["alarm"] = campos["EXTRUDER_ALARM"]
            elif "ALARM" in campos:
                self.telemetria["alarm"] = campos["ALARM"]

            for clave_uart, clave_local in valores_float.items():
                if clave_uart in campos:
                    self.telemetria[clave_local] = float(campos[clave_uart])

        except ValueError:
            pass

    # -- Envía un mensaje a través del puerto UART --
    def enviar(self, comando, timeout=1.0):
        """
        Descripción: Envía un comando a la unidad de control a través del puerto UART y espera una respuesta. Si la comunicación no está disponible o si ocurre un error al enviar el mensaje, se devuelve False. Si se recibe una respuesta, se devuelve True junto con la respuesta.
        Parámetros:
            comando (str): Comando a enviar a la unidad de control.
            timeout (float): Tiempo máximo en segundos para esperar una respuesta.
        Retorna:
            tuple: (bool, str o None) - True si se recibió una respuesta, False si no se recibió respuesta o hubo un error. La segunda parte de la tupla es la respuesta
        """
        if not self.disponible():
            return False, None

        if not self.uart.enviar_mensaje(comando):
            return False, None

        respuesta = self.esperar_respuesta(timeout)
        ok = respuesta is not None and respuesta.startswith("OK")

        self._actualizar_telemetria_desde_texto(respuesta)
        if ok:
            self._actualizar_estado_estimado_por_comando(comando)

        return ok, respuesta

    # -- Envía un comando sin esperar respuesta --
    def enviar_sin_respuesta(self, comando):
        """
        Descripción: Envía un comando a la unidad de control a través del puerto UART sin esperar una respuesta. Esto es útil para comandos que no requieren confirmación inmediata.
        Parámetros:
            comando (str): Comando a enviar a la unidad de control.
        Retorna:
            bool: True si el comando se envió correctamente, False si hubo un error al enviar el comando o si la comunicación no está disponible.
        """
        if self.disponible():
            return self.uart.enviar_mensaje(comando)
        return False

    # -- Espera una respuesta de la unidad de control dentro de un tiempo límite --
    def esperar_respuesta(self, timeout=1.0):
        """
        Descripción: Espera una respuesta de la unidad de control dentro de un tiempo límite.
        Parámetros:
            timeout (float): Tiempo máximo en segundos para esperar una respuesta.
        Retorna:
            str o None: La respuesta recibida o None si no se recibió respuesta.
        """
        fin = time.time() + timeout

        while time.time() < fin:
            if self.respuestas_pendientes:
                return self.respuestas_pendientes.pop(0)

            linea = self.uart.get_mensaje(timeout=0.05)
            if linea is None:
                continue

            linea = linea.strip()

            if linea.startswith("OK") or linea.startswith("ERR"):
                return linea

            self.procesar_linea(linea)

        return None

    # -- Consulta el estado de la unidad de control y extrae el valor del estado --
    def consultar_estado(self, timeout=1.0):
        """
        Descripción: Consulta el estado de la unidad de control enviando el comando "STATE?" y espera una respuesta. Extrae el valor del estado de la respuesta recibida.
        Parámetros:
            timeout (float): Tiempo máximo en segundos para esperar una respuesta.
        Retorna:

        """
        ok, respuesta = self.enviar("STATE?", timeout=timeout)
        self._actualizar_telemetria_desde_texto(respuesta)
        estado = extraer_estado_respuesta(respuesta)
        estado_winder = extraer_estado_winder_respuesta(respuesta)

        if estado is not None:
            self.telemetria["state"] = estado
        if estado_winder is not None:
            self.telemetria["winder_state"] = estado_winder

        return ok and estado is not None, estado, respuesta
    
    # -- Pide la telemetría --
    def pedir_telemetria(self):
        """
        Descripción: Pide la telemetría al uC enviando el comando "TEL?" sin bloquear la HMI. Esto permite que la HMI continúe funcionando mientras se espera la respuesta de la unidad de control.
        Parámetros:
            None
        Retorna:
            bool: True si el comando se envió correctamente, False si hubo un error al enviar el comando o si la comunicación no está disponible.
        """
        if self.disponible():
            return self.uart.enviar_mensaje("TEL?")
        return False

# -- Extrae el estado de la respuesta recibida desde la unidad de control --
def extraer_estado_respuesta(respuesta):
    """
    Descripción: Extrae el estado de la respuesta recibida desde la unidad de control. La respuesta debe contener el campo "STATE=" seguido del valor del estado.
    Parámetros:
        respuesta (str): Respuesta recibida desde la unidad de control.
    Retorna:
        int o None: Valor del estado extraído o None si no se pudo extraer el estado.
    """
    if not respuesta or "STATE=" not in respuesta:
        return None

    try:
        return int(respuesta.split("STATE=", 1)[1].split(";", 1)[0])
    except (ValueError, IndexError):
        return None

# -- Extrae el estado de la enrolladora de la respuesta recibida desde la unidad de control --
def extraer_estado_winder_respuesta(respuesta):
    """
    Descripción: Extrae el estado de la enrolladora de la respuesta recibida desde la unidad de control. La respuesta debe contener el campo "WINDER_STATE=" seguido del valor del estado de la enrolladora.
    Parámetros:
        respuesta (str): Respuesta recibida desde la unidad de control.
    Retorna:
        int o None: Valor del estado de la enrolladora extraído o None si no se pudo extraer el estado de la enrolladora.
    """
    if not respuesta or "WINDER_STATE=" not in respuesta:
        return None

    try:
        return int(respuesta.split("WINDER_STATE=", 1)[1].split(";", 1)[0])
    except (ValueError, IndexError):
        return None

# -- Función para obtener el nombre del estado de la máquina --
def nombre_estado(estado):
    """
    Descripción: Obtiene el nombre del estado de la máquina a partir del valor del estado. Si el estado no se encuentra en el diccionario de nombres de estados, devuelve "DESCONOCIDO" seguido del valor del estado.
    Parámetros:
        estado (int): Valor del estado de la máquina.
    Retorna:
        str: Nombre del estado de la máquina o "DESCONOCIDO" si el estado no se encuentra en el diccionario.
    """
    return NOMBRES_ESTADOS_STM32.get(estado, f"DESCONOCIDO({estado})")

# -- Función para obtener el nombre del estado de la enrolladora --
def nombre_estado_winder(estado):
    """
    Descripción: Obtiene el nombre del estado de la enrolladora a partir del valor del estado. Si el estado no se encuentra en el diccionario de nombres de estados de la enrolladora, devuelve "DESCONOCIDO" seguido del valor del estado.
    Parámetros:
        estado (int): Valor del estado de la enrolladora.
    Retorna:
        str: Nombre del estado de la enrolladora o "DESCONOCIDO" si el estado no se encuentra en el diccionario.
    """
    return NOMBRES_ESTADOS_WINDER.get(estado, f"DESCONOCIDO({estado})")

# -- Función para determinar la fase de la FSM automática a partir del estado actual --
def fase_desde_estado(estado, fase_actual=0):
    """
    Descripción: Determina la fase de la FSM automática a partir del estado actual.
    Parámetros:
        estado (int): Valor del estado de la máquina.
        fase_actual (int): Fase actual de la FSM.
    Retorna:
        int: Fase determinada de la FSM.
    """
    fase_actual = fase_actual or 0

    if estado == MACHINE_IDLE_AUTO:
        return max(fase_actual, 0)

    if estado == MACHINE_PREHEAT:
        return max(fase_actual, 1)

    if estado == MACHINE_EXTRUDING:
        return max(fase_actual, 3)

    if estado == MACHINE_STOP:
        return 4 if fase_actual >= 3 else fase_actual

    return fase_actual