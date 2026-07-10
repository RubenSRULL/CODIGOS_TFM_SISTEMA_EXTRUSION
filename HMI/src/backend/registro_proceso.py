# Autor: Ruben Sahuquillo Redondo
# Descripción: Clase para guardar datos de telemetría y eventos en archivos CSV.

# ============================ #
# Librerías #
# ============================ #
import csv
import os
import time
import threading
from datetime import datetime

# ============================ #
# Clase RegistroProceso #
# ============================ #
class RegistroProceso:
    # --- Constructor --- #
    def __init__(self, carpeta="logs", periodo_telemetria=0.1):
        """
        Descripción:
            Inicializa la clase RegistroProceso, creando los archivos CSV para telemetría y eventos en la carpeta especificada
        Parámetros:
            carpeta: str, ruta de la carpeta donde se guardarán los archivos CSV.
            periodo_telemetria: float, intervalo de tiempo en segundos para guardar muestras de telemetría.
        Retorna:
            None
        """
        self.carpeta = carpeta
        self.periodo_telemetria = periodo_telemetria
        self._ultimo_guardado_telemetria = 0.0
        self._lock = threading.Lock()

        os.makedirs(self.carpeta, exist_ok=True)

        marca = datetime.now().strftime("%Y%m%d_%H%M%S")
        self.ruta_telemetria = os.path.join(self.carpeta, f"telemetria_{marca}.csv")
        self.ruta_eventos = os.path.join(self.carpeta, f"eventos_{marca}.csv")

        self.columnas_telemetria = [
            "timestamp",
            "tiempo_s",
            "diametro_camara_mm",
            "diametro_telemetria_mm",
            "temp",
            "target",
            "heater",
            "rpm_int",
            "rpm_ext",
            "state",
            "alarm",
            "winder_state",
            "winder_alarm",
            "factor_mm_por_pixel",
            "x_laser",
            "ancho_roi",
        ]

        self.columnas_eventos = [
            "timestamp",
            "tiempo_s",
            "tipo",
            "origen",
            "comando",
            "exito",
            "respuesta",
            "detalle",
        ]

        self._t0 = time.time()

        self._crear_csv(self.ruta_telemetria, self.columnas_telemetria)
        self._crear_csv(self.ruta_eventos, self.columnas_eventos)

    # -- Método privado para crear archivos CSV -- #
    def _crear_csv(self, ruta, columnas):
        """
        Descripción: Crea un archivo CSV con las columnas especificadas.
        Parámetros:
            ruta: str, ruta del archivo CSV a crear.
            columnas: list, lista de nombres de columnas para el archivo CSV.
        Retorna:
            None
        """
        with open(ruta, "w", newline="", encoding="utf-8") as f:
            writer = csv.DictWriter(f, fieldnames=columnas)
            writer.writeheader()

    # -- Método para guardar telemetría -- #
    def guardar_telemetria(self, diametro_camara_mm, telemetria, camara=None):
        """
        Descripción: Guarda una muestra de telemetría en el archivo CSV correspondiente, si ha pasado el tiempo suficiente desde la última muestra guardada.
        Parámetros:
            diametro_camara_mm: float, diámetro de la cámara en milímetros.
            telemetria: dict, diccionario con los datos de telemetría a guardar.
            camara: objeto opcional que puede contener información adicional sobre la cámara.
        Retorna:
            None
        """

        ahora = time.time()

        if ahora - self._ultimo_guardado_telemetria < self.periodo_telemetria:
            return

        self._ultimo_guardado_telemetria = ahora

        fila = {
            "timestamp": datetime.now().isoformat(timespec="milliseconds"),
            "tiempo_s": round(time.time() - self._t0, 3),
            "diametro_camara_mm": diametro_camara_mm,
            "diametro_telemetria_mm": telemetria.get("diam"),
            "temp": telemetria.get("temp"),
            "target": telemetria.get("target"),
            "heater": telemetria.get("heater"),
            "rpm_int": telemetria.get("rpm_int"),
            "rpm_ext": telemetria.get("rpm_ext"),
            "state": telemetria.get("state"),
            "alarm": telemetria.get("alarm"),
            "winder_state": telemetria.get("winder_state"),
            "winder_alarm": telemetria.get("winder_alarm"),
            "factor_mm_por_pixel": getattr(camara, "factor_mm_por_pixel", None),
            "x_laser": getattr(camara, "x_laser", None),
            "ancho_roi": getattr(camara, "ancho_roi", None),
        }

        self._escribir_fila(self.ruta_telemetria, self.columnas_telemetria, fila)

    # -- Método para guardar eventos -- #
    def guardar_evento(self, tipo, origen="", comando="", exito="", respuesta="", detalle=""):
        """
        Descripción: Guarda eventos puntuales: botones, comandos, cambios de vista, calibración, parada, etc.
        Parámetros:
            tipo: str, tipo de evento.
            origen: str, origen del evento.
            comando: str, comando asociado al evento.
            exito: str, resultado del evento.
            respuesta: str, respuesta del sistema.
            detalle: str, detalles adicionales del evento.
        Retorna:
            None
        """

        fila = {
            "timestamp": datetime.now().isoformat(timespec="milliseconds"),
            "tiempo_s": round(time.time() - self._t0, 3),
            "tipo": tipo,
            "origen": origen,
            "comando": comando,
            "exito": exito,
            "respuesta": respuesta,
            "detalle": detalle,
        }

        self._escribir_fila(self.ruta_eventos, self.columnas_eventos, fila)

    # -- Método privado para escribir una fila en un archivo CSV -- #
    def _escribir_fila(self, ruta, columnas, fila):
        """
        Descripción: Escribe una fila en el archivo CSV especificado, asegurando la exclusión mutua mediante un bloqueo.
        Parámetros:
            ruta: str, ruta del archivo CSV donde se escribirá la fila.
            columnas: list, lista de nombres de columnas para el archivo CSV.
            fila: dict, diccionario con los datos de la fila a escribir.
        Retorna:
            None
        """
        with self._lock:
            try:
                with open(ruta, "a", newline="", encoding="utf-8") as f:
                    writer = csv.DictWriter(f, fieldnames=columnas)
                    writer.writerow(fila)
            except Exception as e:
                print(f"Error guardando registro en {ruta}: {e}")
