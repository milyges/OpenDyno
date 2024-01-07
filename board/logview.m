rawdata = dlmread("/home/milyges/Projekty/OpenDyno/board/data.txt", ";");

time = rawdata(2:end,1);
speedfiltered = rawdata(2:end,2);
speedm = rawdata(2:end,3);
rawspeed = rawdata(2:end,4);

% Konwersja na m/s
for i = 1:size(time)(1)
  speedm(i) = speedm(i) / 1000;
  speedfiltered(i) = speedfiltered(i) / 1000;
  rawspeed(i) = rawspeed(i) / 1000;
  time(i) = time(i) / 1000;

  % Zaokrąglenie do 0.01m/s
end

subplot(3,1,1);
plot(time, speedm, 'r', time, rawspeed, 'g', time, speedfiltered, 'b');
title('Prędkość');
xlabel('Czas');
ylabel('Prędkośc [m/s]');
legend('Wartośc uśredniona z 3 próbek', 'Wartośc surowa', 'Wartość po filtrze');


% Obliczamy przyspieszenie
acc = zeros(1, size(time));
for i = 2:size(time)(1)
  acc(i) = (speedfiltered(i) - speedfiltered(i - 1)) / (time(i) - time(i-1));
  %acc(i) = (rawspeed(i) - rawspeed(i - 1)) / (time(i) - time(i-1));
  %acc(i) = (speedm(i) - speedm(i - 1)) / (time(i) - time(i-1));
end

% Uśrednienie przyśpieszenia
accm = zeros(1, size(time));
accm(1) = acc(1);
accm(2) = acc(2);

for i = 3:size(time)(1)
  accm(i - 1) = (acc(i - 2) + acc(i - 1) + acc(i)) / 3;
end

% Filtr przyśpieszenia
accfiltered = zeros(1, size(time));
accfiltered(1) = accm(1);
accfiltered(2) = accm(2);

std_dev = 8;
dt = 0.1;
KALMAN_A = 1;
KALMAN_C = 1;
KALMAN_V = 5 * std_dev * dt;
KALMAN_W = std_dev * std_dev;
KALMAN_x0 = accm(1);
KALMAN_P0 = 1;
KALMAN_xpri = KALMAN_x0;
KALMAN_Ppri = KALMAN_P0;
KALMAN_xpost = KALMAN_x0;
KALMAN_Ppost = KALMAN_P0;

for i = 2:size(time)(1)
    %accfiltered(i - 1) = 0.4 * accfiltered(i - 2) + 0.3 * accm(i - 1) + 0.3 * accm(i - 2);

    KALMAN_xpri = KALMAN_A * KALMAN_xpost;
    KALMAN_Ppri = KALMAN_A * KALMAN_Ppost * KALMAN_A' + KALMAN_V;

    eps = accm(i) - KALMAN_C * KALMAN_xpri;
    KALMAN_S = KALMAN_C * KALMAN_Ppri * KALMAN_C' + KALMAN_W;
    KALMAN_K = KALMAN_Ppri * KALMAN_C' * KALMAN_S^(-1);
    KALMAN_xpost = KALMAN_xpri + KALMAN_K * eps;
    KALMAN_Ppost = KALMAN_Ppri - KALMAN_K * KALMAN_S * KALMAN_K';

    accfiltered(i) = KALMAN_xpost;
end


subplot(3,1,2);
plot(time(2:end), acc(2:end), 'g', time(2:end), accm(2:end), 'b', time(2:end), accfiltered(2:end), 'r');
title('Przyspieszenie');
xlabel('Czas');
ylabel('Przyspieszenie [m/s^2]');
legend('Wartość surowa', 'Wartośc uśredniona z 3 próbek');

% moc na kołach
pwr = [ 0 ];

for idx = 2:size(time)(1)
  f = 1460 * accfiltered(idx)
  y = (f * speedfiltered(idx) / 1000);
  pwr = [ pwr;  y ];
end;

subplot(3,1,3)
plot(speedfiltered(2:end), pwr(2:end), 'r')
legend('Moc na kołach')
xlabel('Predkosc [m/s]');
ylabel('Moc [KM]');
