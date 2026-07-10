# * Este fichero ha sido elaborado con ayuda de IA
# Descripción: Funciones para generar SVGs de los diagramas de estados de la extrusora y la enrolladora, con el estado actual resaltado.

# ========================== #
# Librerías
# ========================== #
from html import escape
from pathlib import Path


# ==========================================================
# Mapeo directo de state_id Simulink -> nodos draw.io del SVG
# ==========================================================
#
# EXTRUDER_FSM_Y.state_id:
#   0 BOOT       -> BOOT
#   1 STOP       -> STOP
#   2 IDLE_AUTO  -> IDLE_AUTO
#   3 PREHEAT    -> PREHEAT
#   4 EXTRUDING  -> EXTRUDING
#   5 MANUAL     -> MANUAL
#   6 ALARM      -> ALARM
#
# WINDER_FSM_Y.state_id:
#   0 BOOT       -> BOOT
#   1 STOP       -> STOP
#   2 HOME       -> HOME
#   3 RUNNING    -> RUNNING
#   4 ALARM      -> ALARM
# ==========================================================

EXTRUDER_CELL_IDS = {
    0: "5h7ZzsJRKUr04c_QEV-G-1",  # BOOT
    1: "5h7ZzsJRKUr04c_QEV-G-3",  # STOP
    2: "5h7ZzsJRKUr04c_QEV-G-6",  # IDLE_AUTO
    3: "5h7ZzsJRKUr04c_QEV-G-7",  # PREHEAT
    4: "5h7ZzsJRKUr04c_QEV-G-8",  # EXTRUDING
    5: "5h7ZzsJRKUr04c_QEV-G-5",  # MANUAL
    6: "5h7ZzsJRKUr04c_QEV-G-4",  # ALARM
}

WINDER_CELL_IDS = {
    0: "ddRSOA4oobAeFci0r3gr-3",   # BOOT
    1: "ddRSOA4oobAeFci0r3gr-8",   # STOP
    2: "ddRSOA4oobAeFci0r3gr-18",  # HOME
    3: "ddRSOA4oobAeFci0r3gr-25",  # RUNNING
    4: "ddRSOA4oobAeFci0r3gr-11",  # ALARM
}

EXTRUDER_NAMES = {
    0: "BOOT",
    1: "STOP",
    2: "IDLE_AUTO",
    3: "PREHEAT",
    4: "EXTRUDING",
    5: "MANUAL",
    6: "ALARM",
}

WINDER_NAMES = {
    0: "BOOT",
    1: "STOP",
    2: "HOME",
    3: "RUNNING",
    4: "ALARM",
}

# --- Funcion para leer el contenido de un archivo SVG y manejar errores de lectura ---
def _leer_svg(nombre_archivo):
    """
    Descripción: Lee el contenido de un archivo SVG desde la carpeta "assets" y devuelve su contenido como una cadena
    Parámetros:
        nombre_archivo (str): Nombre del archivo SVG a leer
    Retorna:
        str: Contenido del archivo SVG o un SVG de error si no se pudo leer el archivo
    """
    ruta = Path(__file__).resolve().parents[2] / "assets" / nombre_archivo
    try:
        return ruta.read_text(encoding="utf-8")
    except Exception as e:
        return f"""
        <svg xmlns="http://www.w3.org/2000/svg" width="800" height="220">
            <rect width="100%" height="100%" fill="#ffffff"/>
            <text x="20" y="70" font-size="22" fill="red">
                No se pudo cargar {escape(nombre_archivo)}
            </text>
            <text x="20" y="110" font-size="14" fill="black">
                {escape(str(e))}
            </text>
        </svg>
        """

# --- Función para inyectar estilo CSS en el SVG para resaltar el estado activo ---
def _inyectar_estilo(svg, cell_id):
    """
    Descripción: Inyecta un bloque de estilo CSS en el SVG para resaltar el estado activo correspondiente al cell_id proporcionado.
    Parámetros:
        svg (str): Contenido del archivo SVG
        cell_id (str): ID del nodo en el SVG que representa el estado activo
    Retorna:
        str: Contenido del archivo SVG con el estilo CSS inyectado para resaltar el estado activo
    """

    if not cell_id:
        return svg

    css = f"""
    <style id="fsm-active-state-style">
        g[data-cell-id="{cell_id}"] rect,
        g[data-cell-id="{cell_id}"] ellipse,
        g[data-cell-id="{cell_id}"] path {{
            fill: #fff176 !important;
            stroke: #ff9800 !important;
            stroke-width: 4px !important;
            filter: drop-shadow(0px 0px 7px rgba(255, 152, 0, 0.95));
        }}

        g[data-cell-id="{cell_id}"] foreignObject,
        g[data-cell-id="{cell_id}"] foreignObject * {{
            color: #000000 !important;
            font-weight: 800 !important;
        }}
    </style>
    """

    if "<defs/>" in svg:
        return svg.replace("<defs/>", f"<defs/>{css}", 1)

    if "<defs>" in svg:
        return svg.replace("<defs>", f"<defs>{css}", 1)

    pos = svg.find(">")
    if pos != -1:
        return svg[:pos + 1] + css + svg[pos + 1:]

    return css + svg

# --- Función para inyectar una etiqueta de estado actual en el SVG ---
def _inyectar_etiqueta_estado(svg, titulo, estado, nombre):
    """
    Descripción: Inyecta una etiqueta en el SVG que indica el estado actual de la máquina, mostrando el título, el estado y su nombre correspondiente.
    Parámetros:
        svg (str): Contenido del archivo SVG
        titulo (str): Título de la máquina (por ejemplo, "Extrusora" o "Enrolladora")
        estado (int): ID del estado actual
        nombre (str): Nombre del estado actual
    Retorna:
        str: Contenido del archivo SVG con la etiqueta de estado actual inyectada
    """
    texto = escape(f"{titulo}: {estado} - {nombre}")
    etiqueta = f"""
    <!-- estado-actual: {escape(str(estado))} -->
    <g id="fsm-current-state-label">
        <rect x="12" y="10" width="300" height="34" rx="8" ry="8"
              fill="#ffffff" stroke="#ff9800" stroke-width="2" opacity="0.96"/>
        <text x="26" y="33" font-size="18" font-family="Arial, Helvetica, sans-serif"
              fill="#111111" font-weight="700">{texto}</text>
    </g>
    """

    cierre = svg.rfind("</svg>")
    if cierre != -1:
        return svg[:cierre] + etiqueta + svg[cierre:]

    return svg + etiqueta

# --- Función principal para generar el SVG con el estado activo resaltado ---
def _svg_activa(nombre_archivo, estado, ids, nombres, titulo):
    """
    Descripción: Genera un SVG con el estado activo resaltado, inyectando estilo y etiqueta de estado actual.
    Parámetros:
        nombre_archivo (str): Nombre del archivo SVG a leer
        estado (int): ID del estado actual
        ids (dict): Diccionario que mapea los IDs de estado a los IDs de nodo en el SVG
        nombres (dict): Diccionario que mapea los IDs de estado a sus nombres correspondientes
        titulo (str): Título de la máquina (por ejemplo, "Extrusora" o "Enrolladora")
    Retorna:
        str: Contenido del archivo SVG con el estado activo resaltado y la etiqueta de estado actual inyectada
    """
    estado = int(estado)
    nombre = nombres.get(estado, f"DESCONOCIDO({estado})")
    svg = _leer_svg(nombre_archivo)
    svg = _inyectar_estilo(svg, ids.get(estado))
    svg = _inyectar_etiqueta_estado(svg, titulo, estado, nombre)
    return svg

# --- Funciones públicas para generar los SVGs de la extrusora y la enrolladora con el estado activo resaltado ---
def svg_extrusora_activa(estado):
    """
    Descripción: Genera un SVG del diagrama de estados de la extrusora con el estado activo resaltado.
    Parámetros:
        estado (int): ID del estado actual de la extrusora
    Retorna:
        str: Contenido del archivo SVG de la extrusora con el estado activo resaltado y la etiqueta de estado actual inyectada
    """
    return _svg_activa("EXTRUDER_FSM.svg", estado, EXTRUDER_CELL_IDS, EXTRUDER_NAMES, "Extrusora")

# --- Función pública para generar el SVG de la enrolladora con el estado activo resaltado ---
def svg_winder_activa(estado):
    """
    Descripción: Genera un SVG del diagrama de estados de la enrolladora con el estado activo resaltado.
    Parámetros:
        estado (int): ID del estado actual de la enrolladora
    Retorna:
        str: Contenido del archivo SVG de la enrolladora con el estado activo resaltado y la etiqueta de estado actual inyectada
    """
    return _svg_activa("WINDER_FSM.svg", estado, WINDER_CELL_IDS, WINDER_NAMES, "Enrolladora")