String getTimeStamp() {
  String result;
  Process time;
  time.begin("date");
  time.addParameter("+%D %T");  
  //time.addParameter("+%D-%T");  
  time.run();

  while(time.available()>0) {
    char c = time.read();
    if(c != '\n')
      result += c;
  }

  return result;
}
