//
// PRL projekt 2: Enumeration sort
// Autor: Jakub Pastuszek (xpastu00)
//

#include <mpi.h>
#include <iostream>
#include <fstream>

using namespace std;

const char * inputFile = "numbers";

#define REG_EMPTY (-1)
#define REG_INIT 0

#define TAG_REG_X 0
#define TAG_REG_Y 1
#define TAG_REG_Z 2

int main(int argc, char *argv[])
{
    int procs;
    int myId;
	
	int reg_c = REG_INIT;
    int reg_x = REG_EMPTY;
	int reg_y = REG_EMPTY;
	int reg_z = REG_EMPTY;
	
	int number;
	int recv_y;
	
    MPI_Status stat;
	MPI_Request request;

    //MPI INIT
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myId);

	int lastEnumProc = procs - 2;
	int distribProc = procs - 1;
	int enumProcs = procs - 1;
	
    /*
    //Mìøení èasu
    double start, end;
    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();
    */
	
	if ( myId == distribProc ) 
	{
        fstream fin;

        fin.open(inputFile, ios::in);

        for ( int i = 0; fin.good() && i < enumProcs; ++i ) 
		{
            number = fin.get();
			
            if ( fin.eof() ) 
			{
                break;
			}
			
			if ( i != 0 ) 
			{
                cout << " ";
			}
			
            cout << number;

			MPI_Send(&number, 1, MPI_INT, i, TAG_REG_X, MPI_COMM_WORLD);
			MPI_Send(&number, 1, MPI_INT, 0, TAG_REG_Y, MPI_COMM_WORLD);
        }
		
        fin.close();
        cout << endl << flush;
    }
	else
	{
        MPI_Recv(&reg_x, 1, MPI_INT, distribProc, TAG_REG_X, MPI_COMM_WORLD, &stat);
		
		recv_y = ( myId == 0 ) ? distribProc : myId - 1;
		
		if ( myId != lastEnumProc )
		{
			for ( int i = 0; i < enumProcs; ++i )
			{		
				if ( i != 0 ) 
				{
					MPI_Send(&reg_y, 1, MPI_INT, myId+1, TAG_REG_Y, MPI_COMM_WORLD);
				}
				
				MPI_Recv(&reg_y, 1, MPI_INT, recv_y, TAG_REG_Y, MPI_COMM_WORLD, &stat);
						
				if ( reg_x != REG_EMPTY && reg_y != REG_EMPTY && reg_x > reg_y ) 
				{
					++reg_c;
				}
			}
		
			MPI_Send(&reg_y, 1, MPI_INT, myId+1, TAG_REG_Y, MPI_COMM_WORLD);
		}
		else
		{
			for ( int i = 0; i < enumProcs; ++i )
			{		
				MPI_Recv(&reg_y, 1, MPI_INT, myId-1, TAG_REG_Y, MPI_COMM_WORLD, &stat);
				
				if ( reg_x != REG_EMPTY && reg_y != REG_EMPTY && reg_x > reg_y ) 
				{
					++reg_c;
				}
			}
		}
    }
	
	for ( int i = 0; i < enumProcs; ++i ) 
	{
		MPI_Barrier(MPI_COMM_WORLD);
		
		if ( myId == i )
		{
			if ( myId != reg_c )
			{
				number = reg_c;
			}
			else
			{
				number = REG_EMPTY;
				reg_z = reg_x;
			}	
		}
		else
		{
			number = REG_EMPTY;
		}
		
		MPI_Bcast(&number, 1, MPI_INT, i, MPI_COMM_WORLD);
		
		if ( myId == i && myId != reg_c )
		{
			MPI_Send(&reg_x, 1, MPI_INT, number, TAG_REG_Z, MPI_COMM_WORLD);
		}
		
		if ( myId == number )
		{
			MPI_Recv(&reg_z, 1, MPI_INT, i, TAG_REG_Z, MPI_COMM_WORLD, &stat);
		}
	}
	
	if ( myId != distribProc )
	{
		int temp;
		for ( int i = 0; i < enumProcs - myId; ++i )
		{
			if ( myId == 0 )
			{
				if ( reg_z == REG_EMPTY )
				{
					number = temp;
				}
				else
				{
					number = reg_z;
				}
				
				MPI_Send(&number, 1, MPI_INT, distribProc, TAG_REG_Z, MPI_COMM_WORLD);
				
				if ( reg_z != REG_EMPTY )
				{
					temp = reg_z;
				}
			}
			else
			{		
				MPI_Send(&reg_z, 1, MPI_INT, myId-1, TAG_REG_Z, MPI_COMM_WORLD);
			}
			
			if ( myId != lastEnumProc && i != (enumProcs - myId - 1) )
			{		
				MPI_Recv(&reg_z, 1, MPI_INT, myId+1, TAG_REG_Z, MPI_COMM_WORLD, &stat);	
			}
		}
	}
	else
	{
		for ( int i = 0; i < enumProcs; ++i )
		{
			MPI_Recv(&reg_z, 1, MPI_INT, 0, TAG_REG_Z, MPI_COMM_WORLD, &stat);	
			
			cout << reg_z << "\n";
		}
	}	
	
    /*
    //Mìøení èasu
    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();
    if (myId == 0) 
	{
        cerr << "Exec time: " << end-start << endl;
    }
    */

    MPI_Finalize();

    return 0;
}