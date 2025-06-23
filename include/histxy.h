//Histogram Object Deffinition
//v1.0.0

#define histxy_h

class histo
{
	private:
		//Variables
		int size=16*4096;
		int* data=new int[size];
		//Functions
		void build()
		{
			for(int i=0; i<size; i++)
			{
				data[i]=0;
			}
			return;
		}
	
	public:
		//Members
		int communicator;
		
		//Constructor
		histo()
		{
			build();
			communicator=-1;
		}
		
		//Functions
		void plug(int bin)
		{
			if(bin!=-1)
			{
				data[bin]++;
				return;
			}
			else
			{
				return;
			}
		}
		
		int get(int bin)
		{
			if(bin!=-1)
			{
				communicator=data[bin];
				return communicator;
			}
			else
			{
				return -1;
			}
		}
		
		//Destructor
		~histo()
		{
			delete[] data;
			data=nullptr;
		}
};
