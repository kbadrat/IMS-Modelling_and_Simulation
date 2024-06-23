//Simulation of ballistics in the military
#include "ballistic_table.h"

int main(int argc, char *argv[])
{
    // Struct initialization.
    InputVariables input;
    CalculatedData calc_data;
    Results results;
    results.speed_flag = false;

    GraphsData graphs_data;
    graphs_data.i = 0;
    
    // Works with command line arguments and fills structure with arguments.
    if (handle_args(argc, argv, &input) == EXIT_FAILURE) 
        return EXIT_FAILURE;

    print_input(&input);
    converter_to(&input);

    // Calculations.
    get_temp_faht(&input, &calc_data);
    get_adjusted_bc(&input, &calc_data);
    get_retardion_coeff(&input, &calc_data);
    get_adj_retard_coeff(&input, &calc_data);
    get_drop_at_zero(&input, &calc_data);

    calculate_table(&input, &calc_data, &results);
    print_all(&input, &calc_data, &results, &graphs_data);
    draw_graphs(&results, &graphs_data, &input);

    return 0;
}

void get_adjusted_bc(InputVariables * data, CalculatedData * calc_data) 
{
    calc_data->adjusted_bc = data->ballistic_coefficient * (460 + calc_data->temp_faht) / 
        (519 - data->altitude / 280) * exp(data->altitude / 31654) * (2 - data->pressure / STANDARD_PRESSURE);
}

void get_retardion_coeff(InputVariables * data, CalculatedData * calc_data)
{
    calc_data->retardation_coeff = data->ballistic_coefficient * MAYEWSKI_CONST * pow(data->muzzle_speed, 0.45);
}

void get_adj_retard_coeff(InputVariables * data, CalculatedData * calc_data)
{
    calc_data->adj_retard_coeff = calc_data->retardation_coeff * (460 + calc_data->temp_faht) / (519 - data->altitude / 280) * exp(data->altitude / 31654) * (2 - data->pressure / STANDARD_PRESSURE);
}

void get_temp_faht(InputVariables * data, CalculatedData * calc_data)
{
    calc_data->temp_faht = (data->temperature * 9 / 5) + 32;
}

void get_drop_at_zero(InputVariables * data, CalculatedData * calc_data)
{
    calc_data->drop_at_zero = pow(((41.68 / data->muzzle_speed) / ((1 / (0 + data->at_zero_range)) - (1 / (calc_data->adj_retard_coeff 
        - (0.75 + 0.00006 * data->at_zero_range) * data->retard_coeff_rate * data->at_zero_range)))), 2);
}

void set_special_range(InputVariables * data, Results * results) 
{
    if (data->special_range != 0.0)
        results->range = data->special_range;
}

void get_speed(InputVariables * data, CalculatedData * calc_data, Results * results)
{
    if (results->speed_flag == false)
    {
        results->speed = data->muzzle_speed * pow((1 - 3 * data->retard_coeff_rate * results->range / calc_data->adj_retard_coeff), 1 / data->retard_coeff_rate);
        results->speed_flag = true;
    }
    else if (results->speed > data->break_velocity)
    {
        results->speed = data->muzzle_speed * pow((1 - 3 * data->retard_coeff_rate * results->range / calc_data->adj_retard_coeff), 1 / data->retard_coeff_rate);
    }
    else
        results->speed = 0.0;
    
}

void get_energy(InputVariables * data, CalculatedData * calc_data, Results * results)
{
    results->energy = data->bullet_weight * pow(results->speed, 2) / 450380;
}

void get_drop(InputVariables * data, CalculatedData * calc_data, Results * results)
{
    results->drop = pow(((41.68 / data->muzzle_speed) / ((1 / (0 + results->range)) - (1 / 
        (calc_data->adj_retard_coeff - (0.75 + 0.00006 * results->range) * data->retard_coeff_rate * results->range)))), 2);
}

void get_path(InputVariables * data, CalculatedData * calc_data, Results * results)
{
    results->path = ((calc_data->drop_at_zero + data->scope_height) * results->range / 
        data->at_zero_range - (results->drop + data->scope_height));
}

void get_elevn(InputVariables * data, CalculatedData * calc_data, Results * results)
{
    results->elevn = -(results->path / results->range / data->in_moa * 100);
}

void get_windage(InputVariables * data, CalculatedData * calc_data, Results * results)
{
    results->windage = (79.2 * results->range * data->wind_speed / data->muzzle_speed / (calc_data->adj_retard_coeff / 
        results->range - 1 - data->retard_coeff_rate)) / results->range / data->in_moa * 100 * sin(data->wind_direction / 12 * 2 * M_PI);
}

void get_time(InputVariables * data, CalculatedData * calc_data, Results * results)
{
    results->time = (calc_data->adj_retard_coeff / data->muzzle_speed) / (1 - data->retard_coeff_rate) * 
        (pow(data->muzzle_speed / results->speed, 1 - data->retard_coeff_rate) - 1);
}

void print_table(InputVariables * data, CalculatedData * calc_data, Results * results, GraphsData * graphs_data) 
{
    converter_from(results);
    if (results->range == 0.0)
    {
        printf("%.f \t\t%.f\t\t%.f\t\t%.2f\t\t%.2f\t\t\t\t\t\t%.2f\n",
            results->range, results->speed, results->energy, results->drop, results->path, results->time);
    }
    else
        printf("%.f \t\t%.f\t\t%.f\t\t%.2f\t\t%.2f\t\t%.2f\t\t%.2f\t\t%.2f\n",
            results->range, results->speed, results->energy, results->drop, results->path, results->elevn, results->windage, results->time);

    if (data->special_range_flag == false)
    {
        graphs_data->range[graphs_data->i] = results->range; 
        graphs_data->speed[graphs_data->i] = results->speed; 
        graphs_data->energy[graphs_data->i] = results->energy;
        graphs_data->drop[graphs_data->i] = results->drop;
        graphs_data->path[graphs_data->i] = results->path;
        graphs_data->elevn[graphs_data->i] = results->elevn;
        graphs_data->windage[graphs_data->i] = results->windage;
        graphs_data->time[graphs_data->i] = results->time;
        graphs_data->i++;
    }
    else
        data->special_range_flag = false;

    converter_to_results(results);
}

void calculate_table(InputVariables * data, CalculatedData * calc_data, Results * results)
{
    get_speed(data, calc_data, results);
    get_energy(data, calc_data, results);
    get_drop(data, calc_data, results);
    get_path(data, calc_data, results);
    
    if (results->range != 0.0)
    {   
        get_elevn(data, calc_data, results);
        get_windage(data, calc_data, results);
    }
    
    get_time(data, calc_data, results);
}

void converter_to(InputVariables * data) 
{
    // m/s to fps
    data->muzzle_speed = data->muzzle_speed * MS_TO_FPS; 
    data->break_velocity = data->break_velocity * MS_TO_FPS;

    // gr to gn
    data->bullet_weight = data->bullet_weight * GR_TO_GN; 

    // m to yd
    if (data->special_range != 0.0)
        data->special_range = data->special_range * M_TO_YD;
    data->start_range = data->start_range * M_TO_YD;
    data->increment_range = data->increment_range * M_TO_YD;
    data->at_zero_range = data->at_zero_range * M_TO_YD;

    // cm to in
    data->scope_height = data->scope_height * CM_TO_IN;

    // mmhg to mbar
    data->pressure = data->pressure * MMHG_TO_MBAR;

    // m to ft
    data->altitude = data->altitude * M_TO_FT;

    // m/s to mph
    data->wind_speed = data->wind_speed * MS_TO_MPH;

    // degrees to oclock
    data->wind_direction = data->wind_direction / DEG_TO_OCLOCK;
}

void converter_to_results(Results * results)
{
    // m/s to fps
    results->speed = results->speed * MS_TO_FPS; 

    // m to yd
    results->range = results->range * M_TO_YD;

    // cm to in
    results->drop = results->drop * CM_TO_IN;
    results->path = results->path * CM_TO_IN;

    // j to ft-lb
    results->windage = results->windage / FT_LB_TO_J;
}

void converter_from(Results * results)
{
    // fps to m/s
    results->speed = results->speed * FPS_TO_M_S;

    // yd to m
    results->range = results->range * YD_TO_M;

    // in to cm
    results->drop = results->drop * IN_TO_CM;
    results->path = results->path * IN_TO_CM;

    // ft-lb to j
    results->energy = results->energy * FT_LB_TO_J;

}

void print_input(InputVariables * data) 
{
    printf("%s\n", data->bullet);
    printf("INPUT DATA:\n");
    printf("%.f\tMuzzle speed (m/s)\n", data->muzzle_speed);
    printf("%.2f\tBullet wt (g)\n", data->bullet_weight);
    printf("%.3f\tBallistic coefficient\n", data->ballistic_coefficient);
    if (data->special_range != 0.0)
        printf("%.f \tSpecial range (m)\n", data->special_range);
    printf("%.f   \tStart range (m)\n", data->start_range);
    printf("%.f  \tIncrement (m)\n", data->increment_range);
    printf("%.f   \tat Zero range (m)\n", data->at_zero_range);
    printf("%.f  \tWind speed (m/s)\n", data->wind_speed);
    printf("%.1f \tWind dir. (deg)\n", data->wind_direction);
    printf("%.1f \tTemp. (Celcius)\n", data->temperature);
    printf("%.f \tAltitude (m)\n", data->altitude);
    printf("%.f\tPressure (mmhg)\n", data->pressure);
    printf("%.2f\tScope Ht. (cm)\n", data->scope_height);
    printf("%.2f\tin./MOA @ 100 yds\n", data->in_moa);
    printf("%.2f\tRetard. Coeff. rate\n", data->retard_coeff_rate);
    printf("%.f\tBreak Velocity (m/s)\n", data->break_velocity);
}

void print_all(InputVariables * data, CalculatedData * calc_data, Results * results, GraphsData * graphs_data)
{
    printf("\nCalculated data & constants:\n");
    printf("%.3f\tAdjusted BC\n", calc_data->adjusted_bc);
    printf("%.f\tRetardation coeff.\n", calc_data->retardation_coeff);
    printf("%.f\tAdj. Retard. Coeff.\n", calc_data->adj_retard_coeff);
    printf("%.f\tStandard pressure\n", STANDARD_PRESSURE);
    printf("%.f  \tMayewski constant\n", MAYEWSKI_CONST);
    printf("%.f  \tTemp. (Faht.)\n", calc_data->temp_faht);
    printf("%.2f\tDrop at Zero\n\n", calc_data->drop_at_zero);

    printf("Range[m]\tSpeed[m/s]\tEnergy[J]\tDrop[cm]\tPath[cm]\tElevn[MOA]\tWindage[MOA]\tTime[sec]\n");

    if (data->special_range != 0.0)
    {
        set_special_range(data, results);
        calculate_table(data, calc_data, results);
        print_table(data, calc_data, results, graphs_data);
    }
    
    results->range = data->start_range;
    while(results->speed != 0.0)
    {
        
        calculate_table(data, calc_data, results);
    
        if(results->speed != 0.0) //speed < break velocity
            print_table(data, calc_data, results, graphs_data);
            

        results->range += data->increment_range;
    }
}

int handle_args(int argc, char **argv, InputVariables * input)
{
    // Initialize variables to default values.
    input->bullet = "";
    input->muzzle_speed = MUZZLE_VELOCITY_DEFAULT;
    input->bullet_weight = B_WEIGHT_DEFAULT;
    input->ballistic_coefficient = BC_DEFAULT;
    input->special_range = SPECIAL_RANGE_DEFAULT;
    input->start_range = START_RANGE_DEFAULT;
    input->increment_range = INC_RANGE_DEFAULT;
    input->at_zero_range = ZERO_RANGE_DEFAULT;
    input->wind_speed = WIND_SPEED_DEFAULT; 
    input->wind_direction = WIND_DIRECTION_DEFAULT;
    input->temperature = TEMP_DEFAULT;
    input->altitude = ALTITUDE_DEFAULT;
    input->pressure = PRESSURE_DEFAULT;
    input->scope_height = SCOPE_HEIGHT_DEFAULT;
    input->in_moa = MOA_DEFAULT;
    input->retard_coeff_rate = SPITZER_BOAT_TAIL;
    input->break_velocity = BREAK_VELOCITY_DEFAULT;
    input->special_range_flag = false;

    // Long arguments.
    static struct option options[] =
            {
                    {"bullet",          optional_argument,       0, 'b'},
                    {"muzzle",          optional_argument,       0, 'm'},
                    {"weight",          optional_argument,       0, 'w'},
                    {"coefficient",     optional_argument,       0, 'c'},
                    {"special",         optional_argument,       0, 'r'},
                    {"start",           optional_argument,       0, 's'},
                    {"increment",       optional_argument,       0, 'i'},
                    {"at_zero",         optional_argument,       0, 'z'},
                    {"wind_speed",      optional_argument,       0, 'n'},
                    {"wind_direction",  optional_argument,       0, 'd'},
                    {"temperature",     optional_argument,       0, 't'},
                    {"altitude",        optional_argument,       0, 'a'},
                    {"pressure",        optional_argument,       0, 'e'},
                    {"scope_height",    optional_argument,       0, 'g'},
                    {"moa",             optional_argument,       0, 'o'},
                    {"rate",            optional_argument,       0, 'l'},
                    {"stop_speed",      optional_argument,       0, 'v'},
                    {"help",            no_argument,             0, 'h'},
                    {NULL, 0, 0, '\0'}
            };

    int opt;
    // Parses arguments and processes them.
    while ((opt = getopt_long(argc, argv, "hb::m::w::c::r::s::i::z::n::d::t:a::e::g:o::l::v::h::", options, NULL)) != -1)
    {
        // Stores an optional argument parameter, if present.
        switch(opt)
        {
            case 'b': 
                if (OPTIONAL_ARGUMENT_IS_PRESENT)
                    input->bullet = optarg;
                else
                {
                    fprintf(stderr, "Option [--bullet|-b] without parameter.\n\n");
                    return EXIT_FAILURE;
                }
                break;
            
            case 'm': 
                if (OPTIONAL_ARGUMENT_IS_PRESENT)
                {
                    input->muzzle_speed = atof(optarg);
                    if (input->muzzle_speed >= MUZZLE_VELOCITY_MIN && input->muzzle_speed <= MUZZLE_VELOCITY_MAX)
                        break;
                    else
                    {
                        fprintf(stderr, "Muzzle speed must be between 180 and 1500 m/s.\n\n");
                        return EXIT_FAILURE;
                    }
                }
                else
                {
                    fprintf(stderr, "Option [--muzzle|-m] without parameter.\n\n");
                    return EXIT_FAILURE;
                }
                break;
                
            case 'w': 
                if (OPTIONAL_ARGUMENT_IS_PRESENT)
                {
                    input->bullet_weight = atof(optarg);
                    if (input->bullet_weight >= B_WEIGHT_MIN && input->bullet_weight <= B_WEIGHT_MAX)
                        break;
                    else
                    {
                        fprintf(stderr, "Bullet weight must be between 1.1 and 50 g.\n\n");
                        return EXIT_FAILURE;
                    }
                }
                else
                {
                    fprintf(stderr, "Option [--weight|-w] without parameter.\n\n");
                    return EXIT_FAILURE;
                }
                break;
            
            case 'c': 
                if (OPTIONAL_ARGUMENT_IS_PRESENT)
                {

                    input->ballistic_coefficient = atof(optarg);
                    if (input->ballistic_coefficient >= BC_MIN && input->ballistic_coefficient <= BC_MAX)
                        break;
                    else
                    {
                        fprintf(stderr, "Ballistic coefficient must be between 0.12 and 1.00.\n\n");
                        return EXIT_FAILURE;
                    }
                }
                else
                {
                    fprintf(stderr, "Option [--coefficient|-c] without parameter.\n\n");
                    return EXIT_FAILURE;
                }    
                break;

            case 'r': 
                if (OPTIONAL_ARGUMENT_IS_PRESENT)
                {
                    input->special_range = atof(optarg);
                    input->special_range_flag = true;

                    if(input->special_range > START_RANGE_DEFAULT)
                        break;
                    else
                    {
                        fprintf(stderr, "Special range must be more than 0 m.\n\n");
                        return EXIT_FAILURE;
                    }
                }
                else
                {
                    fprintf(stderr, "Option [--special|-r] without parameter.\n\n");
                    return EXIT_FAILURE;
                }
                break;

            case 's': 
                if (OPTIONAL_ARGUMENT_IS_PRESENT)
                {
                    input->start_range = atof(optarg);
                    if (input->start_range >= START_RANGE_DEFAULT)
                        break;
                    else
                    {
                        fprintf(stderr, "Start range must be more than 0 m.\n\n");
                        return EXIT_FAILURE;
                    }
                }
                else
                {
                    fprintf(stderr, "Option [--start|-s] without parameter.\n\n");
                    return EXIT_FAILURE;
                }
                break;

            case 'i': 
                if (OPTIONAL_ARGUMENT_IS_PRESENT)
                {
                    input->increment_range = atof(optarg);
                    if (input->increment_range > START_RANGE_DEFAULT)
                        break;
                    else
                    {
                        fprintf(stderr, "Increment range must be more than 0 m.\n\n");
                        return EXIT_FAILURE;
                    }
                }
                else
                {
                    fprintf(stderr, "Option [--increment|-i] without parameter.\n\n");
                    return EXIT_FAILURE;
                }
                break;
            
            
            case 'z': 
                if (OPTIONAL_ARGUMENT_IS_PRESENT)
                {
                    input->at_zero_range = atof(optarg);
                    if (input->at_zero_range >= ZERO_RANGE_MIN && input->at_zero_range <= ZERO_RANGE_MAX)
                        break;
                    else
                    {
                        fprintf(stderr, "At zero range must be between 50 and 2000 m.\n\n");
                        return EXIT_FAILURE;
                    }
                }
                else
                {
                    fprintf(stderr, "Option [--at_zero|-z] without parameter.\n\n");
                    return EXIT_FAILURE;
                }
                break;
            
            case 'n': 
                if (OPTIONAL_ARGUMENT_IS_PRESENT)
                {
                    input->wind_speed = atof(optarg);
                    if (input->wind_speed >= WIND_SPEED_MIN && input->wind_speed <= WIND_SPEED_MAX)
                        break;
                    else
                    {
                        fprintf(stderr, "Wind speed must be between 0 and 28 m/s.\n\n");
                        return EXIT_FAILURE;
                    }
                }
                else
                {
                    fprintf(stderr, "Option [--wind_speed|-n] without parameter.\n\n");
                    return EXIT_FAILURE;
                }
                break;

            case 'd': 
                if (OPTIONAL_ARGUMENT_IS_PRESENT)
                {
                    input->wind_direction = atof(optarg);
                    if (input->wind_direction >= WIND_DIRECTION_MIN && input->wind_direction <= WIND_DIRECTION_MAX)
                        break;
                    else
                    {
                        fprintf(stderr, "Wind direction must be between 0 and 360 degrees.\n\n");
                        return EXIT_FAILURE;
                    }
                }
                else
                {
                    fprintf(stderr, "Option [--wind_direction|-d] without parameter.\n\n");
                    return EXIT_FAILURE;
                }
                break;

            case 't': 
                if (OPTIONAL_ARGUMENT_IS_PRESENT)
                {
                    input->temperature = atof(optarg);
                    if(input->temperature >= TEMP_MIN && input->temperature <= TEMP_MAX)
                        break;
                    else
                    {
                        fprintf(stderr, "Temperature must be between -50 and 50 degrees Celsius.\n\n");
                        return EXIT_FAILURE;
                    }
                }
                else
                {
                    fprintf(stderr, "Option [--temperature|-t] without parameter.\n\n");
                    return EXIT_FAILURE;
                }
                break;

            case 'a': 
                if (OPTIONAL_ARGUMENT_IS_PRESENT)
                {
                    input->altitude = atof(optarg);
                    if (input->altitude >= ALTITUDE_MIN && input->altitude <= ALTITUDE_MAX)
                        break;
                    else
                    {
                        fprintf(stderr, "Altitude must be between 0 and 4500 m.\n\n");
                        return EXIT_FAILURE;
                    }
                }
                else
                {
                    fprintf(stderr, "Option [--altitude|-a] without parameter.\n\n");
                    return EXIT_FAILURE;
                }
                break;
            
            case 'e': 
                if (OPTIONAL_ARGUMENT_IS_PRESENT)
                {
                    input->pressure = atof(optarg);
                    if (input->pressure >= PRESSURE_MIN && input->pressure <= PRESSURE_MAX)
                        break;
                    else
                    {
                        fprintf(stderr, "Pressure must be between 650 and 812 mmhg.\n\n");
                        return EXIT_FAILURE;
                    }
                }
                else
                {
                    fprintf(stderr, "Option [--pressure|-e] without parameter.\n\n"); 
                    return EXIT_FAILURE;
                }
                break;
                
            case 'g': 
                if (OPTIONAL_ARGUMENT_IS_PRESENT)
                {
                    input->scope_height = atof(optarg);
                }
                else
                {
                    fprintf(stderr, "Option [--scope_height|-g] without parameter.\n\n");
                    return EXIT_FAILURE;
                }
                break;
            
            case 'o': 
                if (OPTIONAL_ARGUMENT_IS_PRESENT)
                {
                    input->in_moa = atof(optarg);
                    if (input->in_moa >= 0.0)
                        break;
                    else
                    {
                        fprintf(stderr, "In MOA must be more than 0.\n\n");
                        return EXIT_FAILURE;
                    }
                }
                else
                {
                    fprintf(stderr, "Option [--moa|-o] without parameter.\n\n");
                    return EXIT_FAILURE;
                }
                break;

            case 'l': 
                if (OPTIONAL_ARGUMENT_IS_PRESENT)
                {
                    input->retard_coeff_rate = atof(optarg);
                    if (input->retard_coeff_rate > 0.0)
                        break;
                    else
                    {
                        fprintf(stderr, "Retard. coeff. rate must be 0.5 (for Spitzer boat-tail) or 0.4 (for flat-based) or figure derived from firing test data.\n\n");
                        return EXIT_FAILURE;
                    }
                }   
                else
                {
                    fprintf(stderr, "Option [--rate|-l] without parameter.\n\n");
                    return EXIT_FAILURE;
                }
                break;

            case 'v': 
                if (OPTIONAL_ARGUMENT_IS_PRESENT)
                {
                    input->break_velocity = atof(optarg);
                    if (input->break_velocity >= 0.0)
                        break;
                    else
                    {
                        fprintf(stderr, "Break velocity must be more than 0.\n\n");
                        return EXIT_FAILURE;
                    }
                } 
                else
                {
                    fprintf(stderr, "Option [--stop_speed|-v] without parameter.\n\n");
                    return EXIT_FAILURE;
                }
                break;

            case 'h': //Help message.
                printf("The program, based on certain parameters, creates a ballistic table and graphics for it.\n\n" 
                        "The program runs in the following forms:\n"
                        "./ballistic_table [options]\n\n"
                        // "./ballistic_table [-b NAME | --bullet NAME] [-m MUZZLE | --muzzle MUZZLE] [-w WEIGHT | --weight WEIGHT] \n"
                        // "[-c BC | --coefficient BC] [-r TARGET | --special TARGET] [-s START | --start START] [-i STEP | --increment STEP] \n"
                        // "[-z ZERO | --at_zero ZERO] [-n SPEED | --wind_speed SPEED ] [-d DIRECTION | --wind_direction DIRECTION] \n" 
                        // "[-t TEMPERATURE | --temperature TEMPERATURE] [-a ALTITUDE | --altitude ALTITUDE ] [-e PRESSURE | --pressure PRESSURE] \n"
                        // "[-g SCOPE_HT | --scope_height SCOPE_HT] [-o MOA | --moa MOA ] [-l RATE | --rate RATE] [-v STOP | --stop_speed STOP] \n\n"
                        
                        "Options:\n"
                        "-b, --bullet NAME\t\t\tThe bullet name.\n"
                        "-m, --muzzle MUZZLE\t\t\tThe velocity in meters per second (m/s) the bullet leaves your firearm barrel. Default is 914.\n"
                        "-w, --weight WEIGHT\t\t\tThe bullet weight in grams (g), should be on the ammo packaging. Default is 6.0.\n"
                        "-c, --coefficient BC\t\t\tThe value of your ballistic coefficient, this should be available from the ammunition manufacturer. Default is 0.500.\n"
                        "-r, --special TARGET\t\t\tThe distance in meters (m) to the target.\n"
                        "-s, --start START\t\t\tThe distance you want the table to begin at, the shortest range you're interested in shooting. Default is 0.\n"
                        "-i, --increment STEP\t\t\tThe increment at which you want the table to rows to progress, shoot for making your table no more than 10-20 rows. Default is 50.\n"
                        "-z, --at_zero ZERO\t\t\tThe range at which your scope is zero'd in. Default is 100.\n"
                        "-n, --wind_speed SPEED\t\t\tThe wind speed in m/s. Default is 3.\n"
                        "-d, --wind_direction DIRECTION\t\tWind direction in degrees. Default is 90.0.\n"
                        "-t, --temperature TEMPERATURE\t\tThe temperature in Celsius. Default is 20.0.\n"
                        "-a, --altitude ALTITUDE\t\t\tYour current altitude in meters. Default is 150.0.\n"
                        "-e, --pressure PRESSURE\t\t\tThe current pressure of your location in mmhg. Default is 750.0.\n"
                        "-g, --scope_height SCOPE_HT\t\tThe distance in cm between your scope and your rifle measured from the center of the bore to the center of the reticle. Default is 4.5.\n"
                        "-o, --moa MOA\t\t\t\tThe value of a clicks in one MOA. Default is 1.05.\n"
                        "-l, --rate RATE\t\t\t\tRetardation coefficient rate (0.5 - Spitzer boat-tail, 0.4 - flat-based, or figure derived from firing test data). Default is 0.5.\n"
                        "-v, --stop_speed STOP\t\t\tSpeed in m/s after which data won't be counted. Default is 400.0.\n");		
                exit(EXIT_SUCCESS);

            default:
                fprintf(stderr, "Use -h|--help for usage.\n");
        }
    }
    return 0; 
}

void draw_graphs(Results * results, GraphsData * graphs_data, InputVariables * data)
{
    size_t length;
    RGBABitmapImageReference *imageRef = CreateRGBABitmapImageReference();
    StringReference *error;
    
    DrawScatterPlot(imageRef, 1000, 700, graphs_data->range, graphs_data->i, graphs_data->speed, graphs_data->i, error);
    double *pngdata = ConvertToPNG(&length, imageRef->image);
    WriteToFile(pngdata, length, "out/graph_range_speed.png");
    
    DrawScatterPlot(imageRef, 1000, 700, graphs_data->range, graphs_data->i, graphs_data->energy, graphs_data->i, error);
    double *pngdata2 = ConvertToPNG(&length, imageRef->image);
    WriteToFile(pngdata2, length, "out/graph_range_energy.png");

    DrawScatterPlot(imageRef, 1000, 700, graphs_data->range, graphs_data->i, graphs_data->drop, graphs_data->i, error);
    double *pngdata3 = ConvertToPNG(&length, imageRef->image);
    WriteToFile(pngdata3, length, "out/graph_range_drop.png");
    
    DrawScatterPlot(imageRef, 1000, 700, graphs_data->range, graphs_data->i, graphs_data->path, graphs_data->i, error);
    double *pngdata4 = ConvertToPNG(&length, imageRef->image);
    WriteToFile(pngdata4, length, "out/graph_range_path.png");


    graphs_data->elevn[0] = 0.0; 
    DrawScatterPlot(imageRef, 1000, 700, graphs_data->range, graphs_data->i, graphs_data->elevn, graphs_data->i, error);
    double *pngdata5 = ConvertToPNG(&length, imageRef->image);
    WriteToFile(pngdata5, length, "out/graph_range_elevn.png");

    graphs_data->windage[0] = 0.0;
    DrawScatterPlot(imageRef, 1000, 700, graphs_data->range, graphs_data->i, graphs_data->windage, graphs_data->i, error);
    double *pngdata6 = ConvertToPNG(&length, imageRef->image);
    WriteToFile(pngdata6, length, "out/graph_range_windage.png");

    DrawScatterPlot(imageRef, 1000, 700, graphs_data->time, graphs_data->i, graphs_data->range, graphs_data->i, error);
    double *pngdata7 = ConvertToPNG(&length, imageRef->image);
    WriteToFile(pngdata7, length, "out/graph_range_time.png");
}
