sudo nmcli connection add type 802-11-wireless con-name sarath_nivas ssid sarath_nivas 802-11-wireless-security.key-mgmt WPA-PSK 802-11-wireless-security.psk land1234
sudo nmcli con mod sarath_nivas connection.autoconnect true
sudo nmcli connection up sarath_nivas
