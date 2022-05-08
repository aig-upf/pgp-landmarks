#ifndef __COMMON_H__
#define __COMMON_H__

//#include<bits/stdc++.h>
#include "stdc++.h"

#define PROGRAM_FREQUENCY 10000
#define INFO false
#define INF 1000000000
//#define PRINT_LANDMARKS_INFO
#define DEADEND_DETECTION false
#define DEFERRED_EVALUATION false
#define EVALUATION_FREQUENCY 3
#define REEVALUATE_OPEN_QUEUE true
#define CARRY_FLAG false
#define CMP_INSTRUCTION_ALLOWED false
#define POINTER_LANDMARKS true
#define COPY_LANDMARKS false // true does not work properly
#define IFS_ALLOWED false
#define TESTS_ALLOWED true
#define NATURAL_ORDERING true

#ifndef VALIDATOR
#define MAX_VAL 100
#else
#define MAX_VAL 1000000000
//#define MAX_STEPS 200
#endif

using namespace std;

#endif
