% Funkcja usunięcia anomalii ze filtrem przyspieszenia:
% 1. Przyspieszenie poprzeczne 
% 2. Przyspieszenie wzdłużne

function [t_clean, x_clean, y_clean] = usun_anomalie(t, x, y, max_a_poprzeczne, max_a_wzdluzne)    
    t_clean = t; 
    x_clean = x; 
    y_clean = y;
    
    while true
        if length(t_clean) < 3
            break;
        end
        
        dt = diff(t_clean);
        dx = diff(x_clean);
        dy = diff(y_clean);
        
        v = sqrt(dx.^2 + dy.^2) ./ dt;
        theta = atan2(dy, dx);
        
        % OBLICZENIE PARAMETRÓW DLA DRUGIEGO ODCINKA (Względem pierwszego)
        dtheta = diff(theta);
        dtheta = mod(dtheta + pi, 2*pi) - pi;
        
        % 1. Poprzeczne (a = v * dtheta / dt)
        a_poprzeczne = v(2:end) .* abs(dtheta) ./ dt(2:end);
        
        % 2. Wzdłużne (a = dv / dt)
        dv = diff(v);
        a_wzdluzne = abs(dv) ./ dt(2:end);
        
        % Szukanie najgorszych kandydatów
        [max_ap_val, max_ap_idx] = max(a_poprzeczne);
        [max_aw_val, max_aw_idx] = max(a_wzdluzne);
        
        % decyzja o usunieciu
        if max_ap_val > max_a_poprzeczne
            idx_do_usuniecia = max_ap_idx + 1;
            
        elseif max_aw_val > max_a_wzdluzne
            idx_do_usuniecia = max_aw_idx + 1; 
      
        else
            % nie ma nic wiecej do uzuniecia - koniec
            break; 
        end
        
        % usunięcia dla wybranego indeksu
        t_clean(idx_do_usuniecia) = [];
        x_clean(idx_do_usuniecia) = [];
        y_clean(idx_do_usuniecia) = [];
    end
end


% odszumienie pomiarów s2
% limit_ap = 20 m/s^2, limit_aw = 10 m/s^2
data_s1 = readtable('sensor1.csv');
data_s2 = readtable('sensor2.csv');

% Wyciagniecie danych z pliku
t_s1 = data_s1{:, 1}; 
x_s1 = data_s1{:, 2};
y_s1 = data_s1{:, 3};

t_s2 = data_s2{:, 1};
x_s2 = data_s2{:, 2};
y_s2 = data_s2{:, 3};

[t_s2_filtered, x_s2_filtered, y_s2_filtered] = usun_anomalie(t_s2, x_s2, y_s2, 20.0, 10.0);

% Wstępne przypisanie indeksów i wyliczenie interpolowanego s1 wzgledem odszumionych pomiarów s2
idx_przed = discretize(t_s2_filtered, t_s1);
idx_po = min(idx_przed + 1, length(t_s1)); 
waga = (t_s2_filtered - t_s1(idx_przed)) ./ (t_s1(idx_po) - t_s1(idx_przed));
waga(isnan(waga)) = 0;

x_s1_interpolowane = x_s1(idx_przed) + waga .* (x_s1(idx_po) - x_s1(idx_przed));
y_s1_interpolowane = y_s1(idx_przed) + waga .* (y_s1(idx_po) - y_s1(idx_przed));

% Obliczenie uchybu w każdym punkcie 
blad_x = x_s2_filtered - x_s1_interpolowane;
blad_y = y_s2_filtered - y_s1_interpolowane;

% Rozciągnięcie czystego błędu (Hybryda: interpolacja liniowa na końcach, spline w środku)

% Wyliczenie błędu dwiema metodami
blad_x_spline = interp1(t_s2_filtered, blad_x, t_s1, 'spline');
blad_y_spline = interp1(t_s2_filtered, blad_y, t_s1, 'spline');

blad_x_lin = interp1(t_s2_filtered, blad_x, t_s1, 'linear', 'extrap');
blad_y_lin = interp1(t_s2_filtered, blad_y, t_s1, 'linear', 'extrap');

% Ustalenie granic czasowych dla przejazdu na okregach
% Używamy drugiego i przedostatniego punktu s2, aby uniknąć anomalii brzegowych
t_start_kola = t_s2_filtered(2);
t_koniec_kola = t_s2_filtered(end-1);

% Przygotowanie wektorów na wynik
blad_x_pelny = zeros(size(t_s1));
blad_y_pelny = zeros(size(t_s1));

% Liniowa dla prostych, spline dla okręgu
for i = 1:length(t_s1)
    if t_s1(i) < t_start_kola || t_s1(i) > t_koniec_kola
        % Na wjeździe i zjeździe kontrujemy interpolacja błedu liniowa
        blad_x_pelny(i) = blad_x_lin(i);
        blad_y_pelny(i) = blad_y_lin(i);
    else
        % Na okręgach interpolacja błędu spline
        blad_x_pelny(i) = blad_x_spline(i);
        blad_y_pelny(i) = blad_y_spline(i);
    end
end

x_skor = x_s1 + blad_x_pelny;
y_skor = y_s1 + blad_y_pelny;

% Interpolacja na danych (siatka z krokiem 1ms)
t_mili = (0.000 : 0.001 : 33.000)'; % zakres podany w zadaniu
x_mili = interp1(t_s1, x_skor, t_mili, 'spline', 'extrap');
y_mili = interp1(t_s1, y_skor, t_mili, 'spline', 'extrap');

% Zapis skorygowanej trajektorii do nowego pliku csv
T_out = table(t_mili, x_mili, y_mili, 'VariableNames', {'timestamp', 'x', 'y'});
writetable(T_out, 'car_position.csv');
disp('Zapisano plik');

% Wykres skorygowanych pomiarów
figure('Name', 'Trajektoria Skidpad - Ostateczna pozycja bolidu');
hold on; grid on; axis equal;

% Wymiary Skidpad
promien_wewn = 15.25 / 2; szerokosc_toru = 3.00;
promien_zewn = promien_wewn + szerokosc_toru; rozstaw_srodkow = 18.25;
theta = linspace(0, 2*pi, 200); Cy = 0; Cx = 0;
srodek_gora_y = Cy + (rozstaw_srodkow / 2); srodek_dol_y  = Cy - (rozstaw_srodkow / 2);

% Rysowanie toru
plot(Cx + promien_wewn*cos(theta), srodek_gora_y + promien_wewn*sin(theta), 'Color', [0.7 0.7 0.7], 'LineWidth', 2, 'DisplayName', 'Granice toru');
plot(Cx + promien_zewn*cos(theta), srodek_gora_y + promien_zewn*sin(theta), 'Color', [0.7 0.7 0.7], 'LineWidth', 2, 'HandleVisibility', 'off');
plot(Cx + promien_wewn*cos(theta), srodek_dol_y + promien_wewn*sin(theta), 'Color', [0.7 0.7 0.7], 'LineWidth', 2, 'HandleVisibility', 'off');
plot(Cx + promien_zewn*cos(theta), srodek_dol_y + promien_zewn*sin(theta), 'Color', [0.7 0.7 0.7], 'LineWidth', 2, 'HandleVisibility', 'off');

% Ostateczna trajektoria
plot(x_mili, y_mili, 'b', 'LineWidth', 1, 'DisplayName', 'Skorygowane pomiary s1');

legend('Location', 'best');
xlabel('Oś X [m]'); ylabel('Oś Y [m]'); 
title('Trajektoria Skidpad - Ostateczna pozycja bolidu');