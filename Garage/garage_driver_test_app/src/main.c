#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#define BUF_LEN 80

int main()
{
    int file_desc;
    int ret_val;
    char state[BUF_LEN],command[BUF_LEN];
    char option;
    int check_state,menu = 0;

    memset(state,0,BUF_LEN);
    memset(command,0,BUF_LEN);

    printf("O - Open garage.\n");
    printf("C - Close garage.\n");
    printf("S - Stop operation.\n");
    printf("G - Check door status.\n");

    while(1)
    {
    	if(menu == 10)
    	{
    		printf("\n\nO - Open garage.\n");
			printf("C - Close garage.\n");
			printf("S - Stop operation.\n");
			printf("G - Check door status.\n\n");
    		menu = 0;
    	}


        printf("Enter option: ");
        scanf(" %c",&option);

        file_desc = open("/dev/garage", O_RDWR);

		if(file_desc < 0)
		{
		    printf("Error, 'dev/garage' not opened\n");
		    return -1;
		}

        ret_val = read(file_desc, state, BUF_LEN);
        state[ret_val] = '\0';

        close(file_desc);

        switch(option)
        {


            case 'O':
            {
                /******************************************************************************/
                /***********************      O P E N      ************************************/
            	/******************************************************************************/

            	check_state = strcmp(state, "Opened"); // Provera da li su vec otvorena
                if(check_state == 0)
                {
                    printf("Garage already opened!\n");
                }


            	/******************************************************************************/
            	check_state = strcmp(state, "Opening"); // Provera da li se vec otvaraju
                if(check_state == 0)
                {
                    printf("Garage already opening!\n");
                }

				/******************************************************************************/

                check_state = strcmp(state,"Closed"); // Ako su zatvorena mogu da se otvore

                if(check_state == 0)
                {
                	file_desc = open("/dev/garage", O_RDWR);

					if(file_desc < 0)
					{
						printf("Error, 'dev/garage' not opened\n");
						return -1;
					}

                    strcpy(command,"Open");
                    ret_val = write(file_desc, command , strlen(command)); // Upisi u driver komandu "Open"

                    printf("Garage opening.\n");

                    close(file_desc);


                }

				/******************************************************************************/

				check_state = strcmp(state,"Obstacle"); // Ako je bila prepreka mozes da otvoris

                if(check_state == 0)
                {
                	file_desc = open("/dev/garage", O_RDWR);

					if(file_desc < 0)
					{
						printf("Error, 'dev/garage' not opened\n");
						return -1;
					}

                    strcpy(command,"Open");
                    ret_val = write(file_desc, command , strlen(command)); // Upisi u driver komandu "Open"

                    printf("Garage opening.\n");

                    close(file_desc);


                }

				/******************************************************************************/

                check_state = strcmp(state, "Closing"); // Ako se zatvaraju mogu da se otvore
                if(check_state == 0)
                {
                	file_desc = open("/dev/garage", O_RDWR);

					if(file_desc < 0)
					{
						printf("Error, 'dev/garage' not opened\n");
						return -1;
					}

                    strcpy(command,"Open");
                    ret_val = write(file_desc, command , BUF_LEN); // Upisi u driver komandu "Open"

                    printf("Garage opening.\n");

                    close(file_desc);
                }

 				/******************************************************************************/

 				check_state = strcmp(state, "Operation stopped"); // Ako su stopirana mogu da se otvore
                if(check_state == 0)
                {
                	file_desc = open("/dev/garage", O_RDWR);

					if(file_desc < 0)
					{
						printf("Error, 'dev/garage' not opened\n");
						return -1;
					}

                    strcpy(command,"Open");
                    ret_val = write(file_desc, command , BUF_LEN); // Upisi u driver komandu "Open"

                    printf("Garage opening.\n");

                    close(file_desc);
                }

                /******************************************************************************/

                break;
            }



            case 'C':
            {
                /******************************************************************************/
                /**************************  C L O S E   **************************************/
            	/******************************************************************************/

                check_state = strcmp(state, "Closed"); // Provera da li su zatvorena
                if(check_state == 0)
                {
                    printf("Garage already closed!\n");
                }


            	/******************************************************************************/

                check_state = strcmp(state, "Closing"); // Provera da li se vec zatvaraju
                if(check_state == 0)
                {
                    printf("Garage already closing!\n");
                }

            	/******************************************************************************/

                check_state = strcmp(state,"Opened"); // Ako su otvorena mogu da se zatvore
                if(check_state == 0)
                {
                	file_desc = open("/dev/garage", O_RDWR);

					if(file_desc < 0)
					{
						printf("Error, 'dev/garage' not opened\n");
						return -1;
					}

                    strcpy(command,"Close");
                    ret_val = write(file_desc, command , BUF_LEN); // Upisi u driver komandu "Close"

                    printf("Garage closes.\n");

                    close(file_desc);
                }


                /******************************************************************************/


                check_state = strcmp(state, "Opening"); // Ako se otvaraju mogu da se zatvore
                if(check_state == 0)
                {
                	file_desc = open("/dev/garage", O_RDWR);

					if(file_desc < 0)
					{
						printf("Error, 'dev/garage' not opened\n");
						return -1;
					}

                    strcpy(command,"Close");
                    ret_val = write(file_desc, command , BUF_LEN); // Upisi u driver komandu "Close"

                    printf("Garage closes.\n");

                    close(file_desc);
                }

                /******************************************************************************/

                check_state = strcmp(state, "Operation stopped"); // Ako su stopirana mogu da se otvore
                if(check_state == 0)
                {
                	file_desc = open("/dev/garage", O_RDWR);

					if(file_desc < 0)
					{
						printf("Error, 'dev/garage' not opened\n");
						return -1;
					}

                    strcpy(command,"Close");
                    ret_val = write(file_desc, command , BUF_LEN); // Upisi u driver komandu "Close"

                    printf("Garage closes.\n");

                    close(file_desc);
                }

                /******************************************************************************/

                check_state = strcmp(state,"Obstacle"); // Ako je bila prepreka ne mozes da zatvoris

                if(check_state == 0)
                {
                 	printf("Unable to close, obstacle detected earlier\n");
                }

				/******************************************************************************/


                break;
            }


            case 'S':
            {
                /******************************************************************************/
                /******************************  S T O P  *************************************/
            	/******************************************************************************/

                check_state = strcmp(state,"Opening"); // Ako se otvaraju mogu da se stopiraju
                if(check_state == 0)
                {
                	file_desc = open("/dev/garage", O_RDWR);

					if(file_desc < 0)
					{
						printf("Error, 'dev/garage' not opened\n");
						return -1;
					}

                    strcpy(command,"Stop");
                    ret_val = write(file_desc, command , BUF_LEN); // Upisi u driver komandu "Stop"

                    printf("Garage stopped.\n");

                    close(file_desc);
                }

                /******************************************************************************/

                check_state = strcmp(state, "Closing"); // Ako se zatvaraju mogu da se stopiraju
                if(check_state == 0)
                {
                	file_desc = open("/dev/garage", O_RDWR);

					if(file_desc < 0)
					{
						printf("Error, 'dev/garage' not opened\n");
						return -1;
					}

                    strcpy(command,"Stop");
                    ret_val = write(file_desc, command , BUF_LEN); // Upisi u driver komandu "Stop"

                    printf("Garage stopped.\n");

                    close(file_desc);
                }

				/******************************************************************************/


                check_state = strcmp(state, "Opened"); // Provera da li su otvorena
                if(check_state == 0)
                {
                    printf("Garage is opened, nothing to stop!\n");
                }

                /******************************************************************************/

                check_state = strcmp(state, "Closed"); // Provera da li su zatvorena
                if(check_state == 0)
                {
                    printf("Garage is closed, nothing to stop!\n");
                }

                /******************************************************************************/

                check_state = strcmp(state,"Obstacle"); // Vec su stala na prepreku

                if(check_state == 0)
                {
                 	printf("Garage already stopped.\n");
                }

                break;
            }


            case 'G':
            {
                /******************************************************************************/
                /************************* C H E C K  S T A T U S  ****************************/
            	/******************************************************************************/


            	file_desc = open("/dev/garage", O_RDWR);

				if(file_desc < 0)
				{
					printf("Error, 'dev/garage' not opened\n");
					return -1;
				}

				check_state = strcmp(state , "Obstacle detected. Garage stopped!");

				if(check_state == 0)
				{
					printf("Garage status: Operation stopped.");
				}
				else
				{
		        	printf("Garage status: ");
		            puts(state);
		            fflush(stdout);
                }

                close(file_desc);

                /******************************************************************************/

                break;
            }


            default :
            {
                printf("Wrong option! Try again\n");
            }
        }

    	menu++;
    }


    return 0;
}
