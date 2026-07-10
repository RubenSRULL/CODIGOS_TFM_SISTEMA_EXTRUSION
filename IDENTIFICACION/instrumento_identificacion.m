% Autor: Ruben Sahuquillo Redondo
% Descripción: Script para la identificación de un sistema de primer orden con retardo

% Funcionamiento:
% 1. Se establece la comunicación con el microcontrolador a través del puerto serial.
% 2. Se envía un comando para iniciar la adquisición de datos.
% 3. Se leen los datos de temperatura y tiempo en tiempo real y se grafican.
% 4. Se guardan los datos en un archivo CSV.


function [K, tau, L] = instrumento_identificacion()
    % ---------------------------------------
    % ----- Configuracion puerto serial -----
    % ---------------------------------------
    delete(serialportfind);
    serialObj = serialport("COM15", 115200);
    configureTerminator(serialObj,"CR/LF");
    flush(serialObj);
    
    % ---------------------------------------
    % ------- Esperar a que uC inicie -------
    % ---------------------------------------
    while true
        data = readline(serialObj);
        if strcmp(data, "OK")
            disp(data);
            break;
        end
    end

    % ---------------------------------------
    % -------- Enviar configuracion ---------
    % ---------------------------------------
    pwm = 0;
    Tmax = 200;
    cmd = sprintf("INICIO,%d,%d", pwm, Tmax);
    writeline(serialObj, cmd);

    % ---------------------------------------
    % --------- Grafica tiepo real ----------
    % ---------------------------------------
    fig = figure();
    hLine = animatedline('Color', 'r', 'LineWidth', 2);
    grid on;
    xlabel('Tiempo (s)');
    ylabel('Temperatura (°C)');

    % ---------------------------------------
    % --------- Vectores de datos -----------
    % ---------------------------------------
    tiempo = [];
    temperatura = [];
    muestra = 1;

    % ---------------------------------------
    % -------- Bucle lectura datos ----------
    % ---------------------------------------
    while ishandle(fig)
        try
            % Leer datos por serial
            data = readline(serialObj);
            % Limpiar datos
            data_limpia = strip(data);
            % Convertir a double
            datos = str2double(split(data_limpia, ','));
            % Si hay 2 datos y no son NaN
            if numel(datos) >= 2 && all(~isnan(datos(1:2)))
                % Extraer datos y mostrarlos
                T_act = datos(1);
                t_act = datos(2);
                fprintf("(%d) [t=%.2f s] -> Temperatura %.2f ºC\n", muestra, t_act, T_act);
                muestra = muestra + 1;
                % Guardar datos en vectores
                tiempo(end+1) = t_act;
                temperatura(end+1) = T_act;
                % Graficar datos
                addpoints(hLine, t_act, T_act);
                drawnow limitrate;
            end
        catch
            break;
        end
    end

    % ---------------------------------------
    % ---------- Mostrar vectores -----------
    % ---------------------------------------
    disp(temperatura)
    disp(tiempo)

    % ---------------------------------------
    % -------- Guardar datos en CSV ---------
    % ---------------------------------------
    tabla = table(tiempo(:), temperatura(:), ...
        'VariableNames', {'Tiempo_s', 'Temperatura_C'});
    nombre = "datos_" + string(datetime('now','Format','yyyyMMdd_HHmmss')) + ".csv";
    writetable(tabla, nombre);
    disp('Datos guardados en datos_experimento.csv')

    % ---------------------------------------
    % --------- Calculo parametros ----------
    % ---------------------------------------
    % Calculo ganancia
    u = pwm;
    T_inicial = temperatura(1);
    T_final = temperatura(end);
    delta_T = T_final - T_inicial;
    K = delta_T / u;
    % Calculo tau (63.2%)
    valor_63 = T_inicial + (0.632 * delta_T);
    indice = find(temperatura >= valor_63, 1);
    tau = tiempo(indice) - tiempo(1);
    % Calculo retardo L
    umbral = T_inicial + 0.5;
    indice_inicio = find(temperatura > umbral, 1);
    L = tiempo(indice_inicio) - tiempo(1);

    % ---------------------------------------
    % --------- Mostrar resultados ----------
    % ---------------------------------------
    disp("Resultados:")
    disp(["K = " num2str(K)])
    disp(["tau = " num2str(tau)])
    disp(["L = " num2str(L)])

    % ---------------------------------------
    % ------------- Calculo FT --------------
    % ---------------------------------------
    disp('Funcion de transferencia aproximada:')
    G = tf(K, [tau 1], 'InputDelay', L)

    % ---------------------------------------
    % ----------- Simular modelo ------------
    % ---------------------------------------
    % Entrada escalón
    u = ones(size(tiempo)) * pwm;
    % Simular la respuesta del modelo
    y_model = lsim(G, u, tiempo) + temperatura(1);
    % Graficar
    fig = figure;
    plot(tiempo, temperatura, 'b', 'LineWidth', 2);
    hold on;
    plot(tiempo, y_model, 'r--', 'LineWidth', 2);
    grid on;
    xlabel('Tiempo (s)');
    ylabel('Temperatura (°C)');
    legend('Sistema real','Modelo Primer Orden');
    title('Comparación: Sistema real vs Modelo');
    nombre2 = replace(nombre, ".csv", ".jpg");
    saveas(fig, nombre2);
end