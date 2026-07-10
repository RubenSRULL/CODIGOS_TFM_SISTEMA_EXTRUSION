# Autor = Rubén Sahuquillo Redondo
# Descripción: Gráfica de la evolución de la temperatura del filamento en función de la posición x, considerando dos ventiladores y dos velocidades diferentes.

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
h_for = 139.7    # W/(m2 K)

# Longitud de cada ventilador
L_vent = 0.092   # m

# Posiciones de inicio y fin
x_ini_vent1 = 0         # m
x_fin_vent1 = L_vent    # m
x_ini_vent2 = L_vent    # m
x_fin_vent2 = 2 * L_vent# m

# Dominio espacial
x = np.linspace(0, x_fin_vent2, 500)

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


# Temperaturas calculadas para cada combinación de coeficiente de convección y velocidad
T_nat_vmin = T_x(x, h_nat, v_min)
T_nat_vmax = T_x(x, h_nat, v_max)
T_for_vmin = T_x(x, h_for, v_min)
T_for_vmax = T_x(x, h_for, v_max)

# Gráfica
plt.figure(figsize=(9, 5))
plt.plot(x, T_nat_vmin, label="Natural, v = 10 inch/min")
plt.plot(x, T_nat_vmax, label="Natural, v = 26 inch/min")
plt.plot(x, T_for_vmin, label="Forzada, v = 10 inch/min")
plt.plot(x, T_for_vmax, label="Forzada, v = 26 inch/min")
# Zonas de ventiladores
plt.axvspan(x_ini_vent1, x_fin_vent1, alpha=0.15, label="Ventilador 1")
plt.axvspan(x_ini_vent2, x_fin_vent2, alpha=0.15, label="Ventilador 2")
# Líneas verticales de inicio y fin de cada ventilador
plt.axvline(x_ini_vent1, linestyle="--", linewidth=1)
plt.axvline(x_fin_vent1, linestyle="--", linewidth=1)
plt.axvline(x_fin_vent2, linestyle="--", linewidth=1)
# Texto en la gráfica
plt.text(0.046, 190, "Ventilador 1", ha="center")
plt.text(0.138, 190, "Ventilador 2", ha="center")
# Etiquetas y leyenda
plt.xlabel("x [m]")
plt.ylabel("T(x) [°C]")
plt.title("Evolución de la temperatura del filamento")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.show()
# Guardar la gráfica
plt.savefig("enfriamiento.png", dpi=300)