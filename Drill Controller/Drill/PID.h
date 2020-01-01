#ifndef PID_H
#define PID_H


void DriveReset();
void DriveBreak(uint8_t);
void DriveRotate(uint8_t);
uint8_t DrivePID(int, int);
void SetFail();
uint8_t GetDiff();

#endif //PID_H
