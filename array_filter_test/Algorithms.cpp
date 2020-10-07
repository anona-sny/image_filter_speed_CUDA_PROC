using namespace std;

#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "stdio.h"
#include <iostream> 
#include <sys/stat.h>
#include <string>
#include <fstream>
#include "AnonaImage.h"
#include <math.h> 

void processCPU(AnonaImageData * data);