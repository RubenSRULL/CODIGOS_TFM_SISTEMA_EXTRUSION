# Autor: Ruben Sahuquillo Redondo
# Descripción: Funciones para generar gráficos con Plotly


# --- Función para crear una figura de líneas ---
def figura_lineas(titulo, x_label, y_label, series):
    """
    Descripción: Crea una figura de líneas con Plotly.
    Parámetros:
        titulo (str): Título del gráfico.
        x_label (str): Etiqueta del eje x.
        y_label (str): Etiqueta del eje y.
        series (list of tuples): Lista de series de datos, donde cada serie es una tupla (nombre, x, y).
    Retorna:
        dict: Diccionario con la configuración de la figura de líneas.
    """
    return {
        "data": [
            {
                "x": x,
                "y": y,
                "type": "scatter",
                "mode": "lines+markers",
                "name": nombre,
            }
            for nombre, x, y in series
        ],
        "layout": {
            "title": {"text": titulo, "font": {"color": "black", "size": 20}},
            "xaxis": {"title": {"text": x_label, "font": {"color": "black"}}, "automargin": True},
            "yaxis": {"title": {"text": y_label, "font": {"color": "black"}}, "automargin": True},
            "template": "plotly_white",
            "margin": {"t": 80, "l": 80, "r": 40, "b": 80},
            "paper_bgcolor": "white",
            "plot_bgcolor": "#f9f9f9",
        },
    }
