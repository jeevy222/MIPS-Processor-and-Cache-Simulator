#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define ADDU 1
#define SUBU 3
#define AND 4
#define OR  5
#define NOR 7
#define MemSize 65536 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.


class RF
{
    public:
        bitset<32> ReadData1, ReadData2; 
     	RF()
    	{ 
          Registers.resize(32);  
          Registers[0] = bitset<32> (0);  
        }
	
        void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
        {   
            // implement the funciton by you.  
            // mycomment - activate writeEnable before writing any kind of data, or else registers will ignore write operation
            // mycomment - ReadData1 & ReadData2 will hold the value we read from registers map via RdReg1 & RdReg2 address variables
            // mycomment - this function will read and write simultaneously, but it does not necessarily have to do both 
            
            if(WrtEnable == 1){
                Registers[WrtReg.to_ulong()] = WrtData;
            } else {
                ReadData1 = Registers[RdReg1.to_ulong()] ;
                ReadData2 = Registers[RdReg2.to_ulong()] ;
            }
                        
         }
		 
	void OutputRF() // write RF results to file
             {
               ofstream rfout;
                  rfout.open("RFresult.txt",std::ios_base::app);
                  if (rfout.is_open())
                  {
                    rfout<<"A state of RF:"<<endl;
                  for (int j = 0; j<32; j++)
                      {        
                        rfout << Registers[j]<<endl;
                      }
                     
                  }
                  else cout<<"Unable to open file";
                  rfout.close();
               
               }     
	private:
            vector<bitset<32> >Registers;
	
};

class ALU
{
      public:
             bitset<32> ALUresult;
             bitset<32> ALUOperation (bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
             {   
                 // implement the ALU operations by you. 
                unsigned int ALUCommand = ALUOP.to_ulong();
                switch (ALUCommand)
                {
                    case ADDU:
                        ALUresult = oprand1.to_ulong() + oprand2.to_ulong() ;
                        break;
                    case SUBU:
                        ALUresult = oprand1.to_ulong() - oprand2.to_ulong() ;
                        break;
                    case AND:
                        ALUresult = (oprand1 & oprand2) ;
                        break;
                    case OR:
                        ALUresult = (oprand1 | oprand2) ;
                        break;
                    case NOR:
                        ALUresult = ~(oprand1 | oprand2) ;
                        break;           
                    default:
                        cout << "Wrong ALU Command Given" << endl;
                        break;
                }

                 return ALUresult;
               }            
};

class INSMem
{
      public:
          bitset<32> Instruction;
          INSMem() // read instruction memory
          {       IMem.resize(MemSize); 
                  ifstream imem;
                  string line;
                  int i=0;
                  imem.open("imem.txt");
                  if (imem.is_open())
                  {
                  while (getline(imem,line))
                     {      
                        IMem[i] = bitset<8>(line);
                        i++;
                     }
                     
                  }
                  else cout<<"Unable to open file";
                  imem.close();
                     
                  }
                  
          bitset<32> ReadMemory (bitset<32> ReadAddress) 
              {    
               // implement by you. (Read the byte at the ReadAddress and the following three byte).
                
                unsigned long StartAddress = ReadAddress.to_ulong();
                Instruction = bitset<32> (
                IMem[StartAddress + 0].to_string() + 
                IMem[StartAddress + 1].to_string() + 
                IMem[StartAddress + 2].to_string() +
                IMem[StartAddress + 3].to_string()
                 ) ;

               return Instruction;     
              }     
      
      private:
           vector<bitset<8> > IMem;
      
};
      
class DataMem    
{
      public:
          bitset<32> readdata;  
          DataMem() // read data memory
          {
             DMem.resize(MemSize); 
             ifstream dmem;
                  string line;
                  int i=0;
                  dmem.open("dmem.txt");
                  if (dmem.is_open())
                  {
                  while (getline(dmem,line))
                       {      
                        DMem[i] = bitset<8>(line);
                        i++;
                       }
                  }
                  else cout<<"Unable to open file";
                  dmem.close();
          
          }  
          bitset<32> MemoryAccess (bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem) 
          {    
               
               // implement by you.

                unsigned long StartAddress = Address.to_ulong();

                if(readmem == 1){
                    readdata = bitset<32> (
                    DMem[StartAddress + 0].to_string() + 
                    DMem[StartAddress + 1].to_string() + 
                    DMem[StartAddress + 2].to_string() +
                    DMem[StartAddress + 3].to_string()
                    ) ;
                }
                else if(writemem == 1){     //since its big endian, FirstByte will have MSBs,  and that MSBs will be stored first in memory
                    string WriteDataString = WriteData.to_string();
                    string FirstByte  = WriteDataString.substr(0,8);
                    string SecondByte = WriteDataString.substr(8,8);
                    string ThirdByte  = WriteDataString.substr(16,8);
                    string FourthByte = WriteDataString.substr(24,8);
                    
                    DMem[StartAddress + 0] = stoi(FirstByte, nullptr, 2);
                    DMem[StartAddress + 1] = stoi(SecondByte, nullptr, 2);
                    DMem[StartAddress + 2] = stoi(ThirdByte, nullptr, 2);
                    DMem[StartAddress + 3] = stoi(FourthByte, nullptr, 2);
                } 

               return readdata;     
          }   
                     
          void OutputDataMem()  // write dmem results to file
          {
               ofstream dmemout;
                  dmemout.open("dmemresult.txt");
                  if (dmemout.is_open())
                  {
                  for (int j = 0; j< 1000; j++)
                       {     
                        dmemout << DMem[j]<<endl;
                       }
                     
                  }
                  else cout<<"Unable to open file";
                  dmemout.close();
               
               }             
      
      private:
           vector<bitset<8> > DMem;
      
};  


   
int main()
{
    RF myRF;
    ALU myALU;
    INSMem myInsMem;
    DataMem myDataMem;

    // my code starts from here
    unsigned long PC = 0 ;              // PC will increment by 4 since it's byte addressable memory accessing 32 bit data
    bitset <32> currentInstruction ;
    string currentInstructionString, opcode, rs, rt, rd, funct, iImmediate, jAddress ;
    unsigned int opcodeInt; 

    while (1)
	{
        // Fetch
        currentInstruction = myInsMem.ReadMemory(PC) ;       // it's okay even if PC is unsigned long, since bitset can be assigned int/long values
        
		// checking if current instruction is halt or not. Iff not, then execute rest of code
        if((~currentInstruction) != 0){
		    
            // decode(Read RF)
            currentInstructionString = currentInstruction.to_string();
            opcode    = currentInstructionString.substr(0,6);      // since it's string, MSB will have index 0
            opcodeInt = stoi(opcode, nullptr, 2) ;

            if (!opcode.compare("000000")) {    // can also do opcodeInt == 0, but this is more fancy ;)
                
                // R Type instructions
                funct = currentInstructionString.substr(29,3);  // we are taking only last 3 digits for comparision in switch later on
                rs    = currentInstructionString.substr(6,5);
                rt    = currentInstructionString.substr(11,5);
                rd    = currentInstructionString.substr(16,5);

                myRF.ReadWrite( bitset<5>(rs), bitset<5>(rt), 0, 0, 0 );
                                
                unsigned int ALUCommand = stoi(funct, nullptr, 2); 
                switch (ALUCommand)
                {
                    case ADDU:
                        myALU.ALUOperation( ADDU , myRF.ReadData1, myRF.ReadData2); 
                        myRF.ReadWrite( 0, 0, bitset<5>(rd), myALU.ALUresult, 1 );
                        break;
                    case SUBU:
                        myALU.ALUOperation( SUBU , myRF.ReadData1, myRF.ReadData2); 
                        myRF.ReadWrite( 0, 0, bitset<5>(rd), myALU.ALUresult, 1 );
                        break;
                    case AND:
                        myALU.ALUOperation( AND , myRF.ReadData1, myRF.ReadData2); 
                        myRF.ReadWrite( 0, 0, bitset<5>(rd), myALU.ALUresult, 1 );
                        break;
                    case OR:
                        myALU.ALUOperation( OR , myRF.ReadData1, myRF.ReadData2); 
                        myRF.ReadWrite( 0, 0, bitset<5>(rd), myALU.ALUresult, 1 );
                        break;
                    case NOR:
                        myALU.ALUOperation( NOR , myRF.ReadData1, myRF.ReadData2); 
                        myRF.ReadWrite( 0, 0, bitset<5>(rd), myALU.ALUresult, 1 );
                        break;           
                    default:
                        cout << "Wrong ALU Command Given" << endl;
                        break;
                }
                PC = PC + 4 ;

            } else if (opcodeInt == 0x09 ) {    // addiu -> rt = rs + signext(imm)

                rt = currentInstructionString.substr(11,5);
                rs = currentInstructionString.substr(6,5);
                iImmediate = currentInstructionString.substr(16,16);
                if(iImmediate[0] == '1'){   // sign extension
                    iImmediate = "1111111111111111" + iImmediate ;
                } else {
                    iImmediate = "0000000000000000" + iImmediate ;
                }
                myRF.ReadWrite( bitset<5>(rs), 0, 0, 0, 0 );
                myALU.ALUOperation( ADDU , myRF.ReadData1, bitset<32>(iImmediate) ); 
                myRF.ReadWrite( 0, 0, bitset<5>(rt), myALU.ALUresult, 1 );
                PC = PC + 4 ;

            } else if (opcodeInt == 0x04) {     // beq -> if (rs == rt) {PC = PC+4+branchAddress } ; branchAddress = 14*signExt + immediate + "00"
                rt = currentInstructionString.substr(11,5);
                rs = currentInstructionString.substr(6,5);
                iImmediate = currentInstructionString.substr(16,16);

                myRF.ReadWrite( bitset<5>(rs), bitset<5>(rt), 0, 0, 0 );
                if (myRF.ReadData1 == myRF.ReadData2) {
                    if(iImmediate[0] == '1'){
                        iImmediate = "11111111111111" + iImmediate + "00" ;
                    } else {
                        iImmediate = "00000000000000" + iImmediate + "00" ;
                    }
                    PC = PC + 4 + stoul(iImmediate, nullptr, 2) ;
                } else {
                    PC = PC + 4;
                    //continue;  - not necessary, as all after this is in conditions, nothing will execute
                }
            } else if (opcodeInt == 0x23) {     // lw -> rt = Memory(rs + signext(imm))
                rt = currentInstructionString.substr(11,5);
                rs = currentInstructionString.substr(6,5);
                iImmediate = currentInstructionString.substr(16,16);
                if(iImmediate[0] == '1'){
                    iImmediate = "1111111111111111" + iImmediate ;
                } else {
                    iImmediate = "0000000000000000" + iImmediate ;
                }

                myRF.ReadWrite( bitset<5>(rs), 0, 0, 0, 0 );
                unsigned long afterOffsetAddress = myRF.ReadData1.to_ulong() + stoul(iImmediate, nullptr, 2) ; 
                myRF.ReadWrite( 0, 0, bitset<5>(rt), myDataMem.MemoryAccess( afterOffsetAddress, 0 , 1, 0 ) , 1);
                PC = PC + 4 ;

            } else if (opcodeInt == 0x2B) {     // sw -> Memory[rs + signext(mem)] = rt
                rt = currentInstructionString.substr(11,5);
                rs = currentInstructionString.substr(6,5);
                iImmediate = currentInstructionString.substr(16,16);
                if(iImmediate[0] == '1'){
                    iImmediate = "1111111111111111" + iImmediate ;
                } else {
                    iImmediate = "0000000000000000" + iImmediate ;
                }

                myRF.ReadWrite( bitset<5>(rs), bitset<5>(rt), 0, 0, 0 );
                unsigned long afterOffsetAddress = myRF.ReadData1.to_ulong() + stoul(iImmediate, nullptr, 2) ; 
                myDataMem.MemoryAccess(afterOffsetAddress, myRF.ReadData2, 0, 1 );
                PC = PC + 4 ;

            } else if (opcodeInt == 0x02) {     // j -> PC = first4bitsof(PC+4) + address(26 bit) + "00"
                bitset<32> tempPC = PC + 4; 
                string msbNibble = tempPC.to_string().substr(0,4) ; 
                jAddress = currentInstructionString.substr(6,26);
                string finalJumpAddress = msbNibble + jAddress + "00" ;
                PC = stoul(finalJumpAddress, nullptr, 2);
            }


        } else {
            break;  // halt instruction - end of program, wind up and go home
        }
        
		
		// Execute
		
		// Read/Write Mem
		
		// Write back to RF
		
        myRF.OutputRF(); // dump RF;    
    }
        myDataMem.OutputDataMem(); // dump data mem
      
        return 0;
        
}
