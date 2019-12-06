

int ChPIDs[200][20] = { 0 };
int PIDs[200] = { -1 };
int index =0;


void addPID(int pid){
  if(index == 0){
    for (int i = 0; i < 200; i++) {
      PIDs[i] = -1;
      for (int j=0;j<20;j++) {
        ChPIDs[i][j] = -1;
      }
    }
  }
  for (int i = 0; i < 200 && PIDs[i] != -1; i++) {
    if(PIDs[i] == pid)
      return;
  }
PIDs[index] = pid;
index++;
index%=200;
}

void addChildPID(int dadPid,int pid){
for (int i = 0; i < 200 && PIDs[i] != -1; i++) {
  if(PIDs[i] == dadPid){
    for (int j = 0; j < 20; j++) {
      if(ChPIDs[i][j] == -1){
        ChPIDs[i][j] = pid;
        return;
      }
    }
  }
}
}
//Acording to question it returns an integer number but it has a specific range so it cant return long numbers... it can be replaced with char array
int getChildrenPIDs(int pid){

  for (int i = 0; i < 200; i++) {
    if(PIDs[i] == pid){
      int index=1;
      int output =0;
      for (int j = 0; j < 20 && ChPIDs[i][j]!= -1; j++) {
        int temp = ChPIDs[i][j];
        do{
          output += index * (temp%10);
          index *= 10;
          temp/=10;
        }
        while (temp != 0);
      }
      return output;
    }
  }
  return -1;
}
