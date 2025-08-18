sudo nmcli connection add type 802-11-wireless con-name sarath_nivas_EXT ssid sarath_nivas_EXT 802-11-wireless-security.key-mgmt WPA-PSK 802-11-wireless-security.psk land1234
sudo nmcli con mod sarath_nivas_EXT connection.autoconnect true
sudo nmcli connection up sarath_nivas_EXT
