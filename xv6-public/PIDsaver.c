

int ChPIDs[200][10] = { 0 };
int PIDs[200] = { -1 };
int index =0;


void addPID(int pid){
  for (int i = 0; i < 50 && PIDs[i] != -1; i++) {
    if(PIDs[i] == pid)
    return;
  }
PIDs[index] = pid;
index++;
}

void addChildPID(int dadPid,int pid){
for (int i = 0; i < 50 && PIDs[i] != -1; i++) {
  if(PIDs[i] == dadPid){
    for (int j = 0; j < 10; j++) {
      if(ChPIDs[i][j] == 0){
        ChPIDs[i][j] = pid;
        return;
      }
    }
  }
}
}

int getChildrenPIDs(int pid){

  for (int i = 0; i < 50; i++) {
    if(PIDs[i] == pid){
      int index=1;
      int output =1;
      for (int j = 0; j < 10 && ChPIDs[i][j]!= 0; j++) {
        output += index * ChPIDs[i][j];
        index *= 10;
      }
      return output;
    }
  }
  return -1;
}
