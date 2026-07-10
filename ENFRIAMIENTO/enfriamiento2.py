# Autor = Rubén Sahuquillo Redondo
# Descripción: Gráfica de la evolución de la temperatura del filamento con convección natural en función de la posición x, considerando dos ventiladores y dos velocidades diferentes.

# =============================== #
# Importación de librerías
# =============================== #
import numpy as np
import matplotlib.pyplot as plt

# Datos
T_inf = 20      # ºC
T_i = 200       # ºC
D = 0.00175     # m
rho = 1240      # kg/m3
cp = 1800       # J/(kg K)

# Velocidades del filamento
v_min = 10 * 0.0254 / 60   # m/s
v_max = 26 * 0.0254 / 60   # m/s

# Coeficientes de convección
h_nat = 24.1     # W/(m2 K)

# Dominio espacial
x = np.linspace(0, 2, 500)

# Función de temperatura
def T_x(x, h, v):
    """
    Descripción: Calcula la temperatura del filamento en función de la posición x, el coeficiente de convección h y la velocidad v.
    Parametros:
        x: Posición a lo largo del filamento [m]
        h: Coeficiente de convección [W/(m2 K)]
        v: Velocidad del filamento [m/s]
    Retorna:
        T: Temperatura del filamento en la posición x [°C]
    """
    return T_inf + (T_i - T_inf) * np.exp(-(4 * h / (rho * cp * v * D)) * x)

# Temperaturas
T_nat_vmin = T_x(x, h_nat, v_min)
T_nat_vmax = T_x(x, h_nat, v_max)

# Gráfica
plt.figure(figsize=(9, 5))
plt.plot(x, T_nat_vmin, label="Natural, v = 10 inch/min")
plt.plot(x, T_nat_vmax, label="Natural, v = 26 inch/min")
# Etiquetas y leyenda
plt.xlabel("x [m]")
plt.ylabel("T(x) [°C]")
plt.title("Evolución de la temperatura del filamento convección natural")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.show()
# Guardar la gráfica
plt.savefig("enfriamiento_natural.png", dpi=300)