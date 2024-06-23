// Authors: Vladyslav Kovalets, Evgeniya Taipova

#ifndef __BALLISTIC_TABLE__
#define __BALLISTIC_TABLE__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#include <math.h>
#include <time.h>
#include "../lib/pbPlots.h"
#include "../lib/supportLib.h"

// Inspired by the macro idea: https://cfengine.com/blog/2021/optional-arguments-with-getopt-long/
// For option with optional argument.
#define OPTIONAL_ARGUMENT_IS_PRESENT \
    ((optarg == NULL && optind < argc && argv[optind][0] != '-') \
     ? (bool) (optarg = argv[optind++]) \
     : (optarg != NULL))


// Default values
#define BC_MIN 0.12
#define BC_MAX 1.00
#define BC_DEFAULT 0.500
#define MUZZLE_VELOCITY_MIN 180.0
#define MUZZLE_VELOCITY_MAX 1500.0
#define MUZZLE_VELOCITY_DEFAULT 914.0
#define B_WEIGHT_MIN 1.1
#define B_WEIGHT_MAX 50.0
#define B_WEIGHT_DEFAULT 6.0
#define START_RANGE_DEFAULT 0.0
#define INC_RANGE_DEFAULT 50.0
#define SPECIAL_RANGE_DEFAULT 0.0
#define WIND_DIRECTION_MIN 0.0
#define WIND_DIRECTION_MAX 360.0
#define WIND_DIRECTION_DEFAULT 90.0
#define WIND_SPEED_MIN 0.0
#define WIND_SPEED_MAX 28.0
#define WIND_SPEED_DEFAULT 3.0
#define TEMP_MIN -50.0
#define TEMP_MAX 50.0
#define TEMP_DEFAULT 20.0
#define PRESSURE_MIN 650.0
#define PRESSURE_MAX 812.0
#define PRESSURE_DEFAULT 750.0
#define ZERO_RANGE_MIN 50.0
#define ZERO_RANGE_MAX 2000.0
#define ZERO_RANGE_DEFAULT 100.0
#define ALTITUDE_MIN 0.0
#define ALTITUDE_MAX 4500.0
#define ALTITUDE_DEFAULT 150.0
#define SPITZER_BOAT_TAIL 0.5
#define SCOPE_HEIGHT_DEFAULT 4.5
#define MOA_DEFAULT 1.05
#define BREAK_VELOCITY_DEFAULT 400.0

#define STANDARD_PRESSURE 1000.0
#define MAYEWSKI_CONST 246.0

#define MS_TO_FPS 3.28084
#define FPS_TO_M_S 0.3048
#define GR_TO_GN 15.4323584
#define M_TO_YD 1.0936133
#define YD_TO_M 0.9144
#define CM_TO_IN 0.393701
#define IN_TO_CM 2.54
#define MMHG_TO_MBAR 1.3332239
#define M_TO_FT 3.28084
#define MS_TO_MPH 2.2369363
#define DEG_TO_OCLOCK 30.0
#define FT_LB_TO_J 1.3558179483314004

#define ARRAY_SIZE 100


// To work with arguments.
typedef struct{
    char *bullet; 
    double muzzle_speed; 
    double bullet_weight; 
    double ballistic_coefficient; 
    double special_range;
    bool   special_range_flag;
    double start_range; 
    double increment_range; 
    double at_zero_range; 
    double wind_speed; 
    double wind_direction; 
    double temperature; 
    double altitude; 
    double pressure; 
    double scope_height; 
    double in_moa; 
    double retard_coeff_rate;
    double break_velocity; 
}InputVariables;

typedef struct {
    double adjusted_bc;
    double retardation_coeff;
    double adj_retard_coeff;
    double temp_faht;
    double drop_at_zero;
}CalculatedData;

typedef struct {
    double range;
    double speed; 
    bool speed_flag;
    double energy;
    double drop;
    double path;
    double elevn;
    double windage;
    double time;
}Results; 

typedef struct {
    double range[ARRAY_SIZE];
    double speed[ARRAY_SIZE];
    double energy[ARRAY_SIZE];
    double drop[ARRAY_SIZE];
    double path[ARRAY_SIZE];
    double elevn[ARRAY_SIZE];
    double windage[ARRAY_SIZE];
    double time[ARRAY_SIZE];
    int i;
}GraphsData;

// Works with command line arguments and fills structure with arguments.
int handle_args(int argc, char **argv, InputVariables * input);
void print_all(InputVariables * data, CalculatedData * calc_data, Results * results, GraphsData * graphs_data); 
void get_adjusted_bc(InputVariables * data, CalculatedData * calc_data);
void get_retardion_coeff(InputVariables * data, CalculatedData * calc_data);
void get_adj_retard_coeff(InputVariables * data, CalculatedData * calc_data);
void get_temp_faht(InputVariables * data, CalculatedData * calc_data);
void get_drop_at_zero(InputVariables * data, CalculatedData * calc_data);

void set_special_range(InputVariables * data, Results * results);
void get_speed(InputVariables * data, CalculatedData * calc_data, Results * results);
void get_energy(InputVariables * data, CalculatedData * calc_data, Results * results);
void get_drop(InputVariables * data, CalculatedData * calc_data, Results * results);
void get_path(InputVariables * data, CalculatedData * calc_data, Results * results);
void get_elevn(InputVariables * data, CalculatedData * calc_data, Results * results);
void get_windage(InputVariables * data, CalculatedData * calc_data, Results * results);
void get_time(InputVariables * data, CalculatedData * calc_data, Results * results);
void calculate_table(InputVariables * data, CalculatedData * calc_data, Results * results);
void converter_to(InputVariables * data);
void converter_to_results(Results * results);
void converter_from(Results * results);
void print_table(InputVariables * data, CalculatedData * calc_data, Results * results, GraphsData * graphs_data);
void print_input(InputVariables * data);
void draw_graphs(Results * results, GraphsData * graphs_data, InputVariables * data); 

#endif //__BALLISTIC_TABLE__