void wifiStatus() {

  Process wifiCheck;
  wifiCheck.runShellCommand("pretty-wifi-info.lua | grep Signal | sed 's/[^0-9]*//g'");
  char wifi_signal[3];

  while (wifiCheck.available() > 0) {
    for (int i = 0; i < 3; i++) {
      wifi_signal[i] = wifiCheck.read(); 
    }
  }

  if(wifi_signal > 0) {
    digitalWrite(Pin7Yellow, HIGH);
  } else {
    digitalWrite(Pin7Yellow, LOW);
  }
}
