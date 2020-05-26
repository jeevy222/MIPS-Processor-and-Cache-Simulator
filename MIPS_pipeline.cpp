#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define MemSize 1000 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

struct IFStruct {
    bitset<32>  PC;
    bool        nop;  
};

struct IDStruct {
    bitset<32>  Instr;
    bool        nop;  
};

struct EXStruct {
    bitset<32>  Read_data1;
    bitset<32>  Read_data2;
    bitset<16>  Imm;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        is_I_type;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        alu_op;     //1 for addu, lw, sw, 0 for subu 
    bool        wrt_enable;
    bool        nop;  
};

struct MEMStruct {
    bitset<32>  ALUresult;
    bitset<32>  Store_data;
    bitset<5>   Rs;
    bitset<5>   Rt;    
    bitset<5>   Wrt_reg_addr;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        wrt_enable;    
    bool        nop;    
};

struct WBStruct {
    bitset<32>  Wrt_data;
    bitset<5>   Rs;
    bitset<5>   Rt;     
    bitset<5>   Wrt_reg_addr;
    bool        wrt_enable;
    bool        nop;     
};

struct stateStruct {
    IFStruct    IF;
    IDStruct    ID;
    EXStruct    EX;
    MEMStruct   MEM;
    WBStruct    WB;
};

class RF
{
    public: 
        bitset<32> Reg_data;
     	RF()
    	{ 
			Registers.resize(32);  
			Registers[0] = bitset<32> (0);  
        }
	
        bitset<32> readRF(bitset<5> Reg_addr)
        {   
            Reg_data = Registers[Reg_addr.to_ulong()];
            return Reg_data;
        }
    
        void writeRF(bitset<5> Reg_addr, bitset<32> Wrt_reg_data)
        {
            Registers[Reg_addr.to_ulong()] = Wrt_reg_data;
        }
		 
		void outputRF()
		{
			ofstream rfout;
			rfout.open("HPRFresult.txt",std::ios_base::app);
			if (rfout.is_open())
			{
				rfout<<"State of RF:\t"<<endl;
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

class INSMem
{
	public:
        bitset<32> Instruction;
        INSMem()
        {       
			IMem.resize(MemSize); 
            ifstream imem;
			string line;
			int i=0;
			imem.open("imem.txt");
			if (imem.is_open())
			{
				while (getline(imem,line))
				{
                    line = line.substr(0,8);
					IMem[i] = bitset<8>(line);
					i++;
				}                    
			}
            else cout<<"Unable to open file";
			imem.close();                     
		}
                  
		bitset<32> readInstr(bitset<32> ReadAddress) 
		{    
			string insmem;
			insmem.append(IMem[ReadAddress.to_ulong()].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+1].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+2].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+3].to_string());
			Instruction = bitset<32>(insmem);		//read instruction memory
			return Instruction;     
		}     
      
    private:
        vector<bitset<8> > IMem;     
};
      
class DataMem    
{
    public:
        bitset<32> ReadData;  
        DataMem()
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
                    line = line.substr(0,8);
                    DMem[i] = bitset<8>(line);
                    i++;
                }
            }
            else cout<<"Unable to open file";
                dmem.close();          
        }
		
        bitset<32> readDataMem(bitset<32> Address)
        {	
			string datamem;
            datamem.append(DMem[Address.to_ulong()].to_string());
            datamem.append(DMem[Address.to_ulong()+1].to_string());
            datamem.append(DMem[Address.to_ulong()+2].to_string());
            datamem.append(DMem[Address.to_ulong()+3].to_string());
            ReadData = bitset<32>(datamem);		//read data memory
            return ReadData;               
		}
            
        void writeDataMem(bitset<32> Address, bitset<32> WriteData)            
        {
            DMem[Address.to_ulong()] = bitset<8>(WriteData.to_string().substr(0,8));
            DMem[Address.to_ulong()+1] = bitset<8>(WriteData.to_string().substr(8,8));
            DMem[Address.to_ulong()+2] = bitset<8>(WriteData.to_string().substr(16,8));
            DMem[Address.to_ulong()+3] = bitset<8>(WriteData.to_string().substr(24,8));  
        }   
                     
        void outputDataMem()
        {
            ofstream dmemout;
            dmemout.open("HPdmemresult.txt");
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

void printState(stateStruct state, int cycle)
{
    ofstream printstate;
    printstate.open("HPstateresult.txt", std::ios_base::app);
    if (printstate.is_open())
    {
        printstate<<"State after executing cycle:\t"<<cycle<<endl; 
        
        printstate<<"IF.PC:\t"<<state.IF.PC.to_ulong()<<endl;        
        printstate<<"IF.nop:\t"<<state.IF.nop<<endl; 
        
        printstate<<"ID.Instr:\t"<<state.ID.Instr<<endl; 
        printstate<<"ID.nop:\t"<<state.ID.nop<<endl;
        
        printstate<<"EX.Read_data1:\t"<<state.EX.Read_data1<<endl;
        printstate<<"EX.Read_data2:\t"<<state.EX.Read_data2<<endl;
        printstate<<"EX.Imm:\t"<<state.EX.Imm<<endl; 
        printstate<<"EX.Rs:\t"<<state.EX.Rs<<endl;
        printstate<<"EX.Rt:\t"<<state.EX.Rt<<endl;
        printstate<<"EX.Wrt_reg_addr:\t"<<state.EX.Wrt_reg_addr<<endl;
        printstate<<"EX.is_I_type:\t"<<state.EX.is_I_type<<endl; 
        printstate<<"EX.rd_mem:\t"<<state.EX.rd_mem<<endl;
        printstate<<"EX.wrt_mem:\t"<<state.EX.wrt_mem<<endl;        
        printstate<<"EX.alu_op:\t"<<state.EX.alu_op<<endl;
        printstate<<"EX.wrt_enable:\t"<<state.EX.wrt_enable<<endl;
        printstate<<"EX.nop:\t"<<state.EX.nop<<endl;        

        printstate<<"MEM.ALUresult:\t"<<state.MEM.ALUresult<<endl;
        printstate<<"MEM.Store_data:\t"<<state.MEM.Store_data<<endl; 
        printstate<<"MEM.Rs:\t"<<state.MEM.Rs<<endl;
        printstate<<"MEM.Rt:\t"<<state.MEM.Rt<<endl;   
        printstate<<"MEM.Wrt_reg_addr:\t"<<state.MEM.Wrt_reg_addr<<endl;              
        printstate<<"MEM.rd_mem:\t"<<state.MEM.rd_mem<<endl;
        printstate<<"MEM.wrt_mem:\t"<<state.MEM.wrt_mem<<endl; 
        printstate<<"MEM.wrt_enable:\t"<<state.MEM.wrt_enable<<endl;         
        printstate<<"MEM.nop:\t"<<state.MEM.nop<<endl;        

        printstate<<"WB.Wrt_data:\t"<<state.WB.Wrt_data<<endl;
        printstate<<"WB.Rs:\t"<<state.WB.Rs<<endl;
        printstate<<"WB.Rt:\t"<<state.WB.Rt<<endl;        
        printstate<<"WB.Wrt_reg_addr:\t"<<state.WB.Wrt_reg_addr<<endl;
        printstate<<"WB.wrt_enable:\t"<<state.WB.wrt_enable<<endl;        
        printstate<<"WB.nop:\t"<<state.WB.nop<<endl; 
    }
    else cout<<"Unable to open file";
    printstate.close();
}

bitset<32> signextend (bitset<16> imm)
{
    string sestring;
    if (imm[15]==0){
        sestring = "0000000000000000"+imm.to_string<char,std::string::traits_type,std::string::allocator_type>();
    }
    else{
        sestring = "1111111111111111"+imm.to_string<char,std::string::traits_type,std::string::allocator_type>();
    }
    return (bitset<32> (sestring));
    
}
 

int main()
{
    
    RF myRF;
    INSMem myInsMem;
    DataMem myDataMem;

    stateStruct state, newState;
    state.WB.nop = 1;
    state.MEM.nop = 1;
    state.EX.nop = 1;
    state.ID.nop = 1;
    state.IF.nop = 0;
    newState.IF.nop = 0 ;
    state.IF.PC = 0;
    
    int stall = 0;
    
    int cycle = 0;
             
    while (1) {

        /* --------------------- WB stage --------------------- */
        if (state.WB.nop == 1){
            // yes. the nop train is completed
            // for now
        } else {
            if(state.WB.wrt_enable == true){
                myRF.writeRF(state.WB.Wrt_reg_addr, state.WB.Wrt_data);
                if(state.WB.Wrt_reg_addr == state.EX.Rs){
                    // MEM-EX forwarding for Rs
                    state.EX.Read_data1 = state.WB.Wrt_data;
                }
                if(state.WB.Wrt_reg_addr == state.EX.Rt){
                    // MEM-EX forwarding for Rt
                    state.EX.Read_data2 = state.WB.Wrt_data;
                }
                if(state.WB.Wrt_reg_addr == state.MEM.Rt){
                    // MEM-MEM forwarding for Rt
                    state.MEM.Store_data = state.WB.Wrt_data;
                }
            }
        }



        /* --------------------- MEM stage --------------------- */
        if (state.MEM.nop == 1){
            newState.WB.nop = state.MEM.nop;
        } else {
            newState.WB.nop = state.MEM.nop;
            if (state.MEM.rd_mem == true) {
                // load instruction - get ALURESULT address from memory
                // R[rt] = mem[R[rs] + signExtImm]
                newState.WB.Wrt_data = myDataMem.readDataMem(state.MEM.ALUresult);
                if(state.MEM.wrt_enable){
                    if(state.MEM.Wrt_reg_addr == state.EX.Rs){
                        // EX-EX forwarding for Rs
                        state.EX.Read_data1 = state.MEM.ALUresult;
                    }
                    if(state.MEM.Wrt_reg_addr == state.EX.Rt){
                        // EX-EX forwarding for Rt
                        state.EX.Read_data2 = state.MEM.ALUresult;
                    }
                }
                newState.WB.Rs = state.MEM.Rs;
                newState.WB.Rt = state.MEM.Rt;
                newState.WB.Wrt_reg_addr = state.MEM.Wrt_reg_addr ;
                newState.WB.wrt_enable = state.MEM.wrt_enable ;
            } else if (state.MEM.wrt_mem == true){
                // store instruction
                // mem[R[rs] + signExtImm] = R[rt]
                myDataMem.writeDataMem(state.MEM.ALUresult, state.MEM.Store_data); // write data to memory
                if(state.MEM.wrt_enable){
                    if(state.MEM.Wrt_reg_addr == state.EX.Rs){
                        // EX-EX forwarding for Rs
                        state.EX.Read_data1 = state.MEM.ALUresult;
                    }
                    if(state.MEM.Wrt_reg_addr == state.EX.Rt){
                        // EX-EX forwarding for Rt
                        state.EX.Read_data2 = state.MEM.ALUresult;
                    }
                }
                newState.WB.Rs = state.MEM.Rs;
                newState.WB.Rt = state.MEM.Rt;
                newState.WB.Wrt_reg_addr = state.MEM.Wrt_reg_addr ;
                newState.WB.wrt_enable = state.MEM.wrt_enable ;
                newState.WB.Wrt_data.reset() ;  // don't care here, because sw doesn't store any data back in register file
            } else {
                // r type instruction - no data memory operations
                newState.WB.Wrt_data = state.MEM.ALUresult ;
                if(state.MEM.wrt_enable){
                    if(state.MEM.Wrt_reg_addr == state.EX.Rs){
                        // EX-EX forwarding for Rs
                        state.EX.Read_data1 = state.MEM.ALUresult;
                    }
                    if(state.MEM.Wrt_reg_addr == state.EX.Rt){
                        // EX-EX forwarding for Rt
                        state.EX.Read_data2 = state.MEM.ALUresult;
                    }
                }
                newState.WB.Rs = state.MEM.Rs ;
                newState.WB.Rt = state.MEM.Rt ;
                newState.WB.Wrt_reg_addr = state.MEM.Wrt_reg_addr ;
                newState.WB.wrt_enable = state.MEM.wrt_enable;
            }
            
            
            
        }


        /* --------------------- EX stage --------------------- */
        if (state.EX.nop == 1){
            newState.MEM.nop = state.EX.nop;
        } else {
            newState.MEM.nop = state.EX.nop;
            if(state.EX.alu_op == true){
                if(state.EX.is_I_type == false){
                    // r type addu instruction
                    newState.MEM.ALUresult = state.EX.Read_data1.to_ulong() + state.EX.Read_data2.to_ulong() ;
                    newState.MEM.Store_data.reset(); // don't care about this value
                    newState.MEM.Rs = state.EX.Rs ;
                    newState.MEM.Rt = state.EX.Rt ;
                    newState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr ;
                    newState.MEM.wrt_enable = state.EX.wrt_enable;
                    newState.MEM.rd_mem = state.EX.rd_mem ;
                    newState.MEM.wrt_mem = state.EX.wrt_mem ;
                    // don't care about rd_mem and wrt_mem
                } else if (state.EX.is_I_type == true) {
                    if(state.EX.rd_mem == true){
                        // I type lw instruction
                        // R[rt] = mem[R[rs] + signExtImm]
                        newState.MEM.ALUresult = state.EX.Read_data1.to_ulong() + signextend(state.EX.Imm).to_ulong() ;
                        newState.MEM.Store_data.reset(); // don't care for lw
                        newState.MEM.Rs = state.EX.Rs ;
                        newState.MEM.Rt = state.EX.Rt ;
                        newState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr ;
                        newState.MEM.wrt_enable = state.EX.wrt_enable;
                        newState.MEM.rd_mem = state.EX.rd_mem;
                        newState.MEM.wrt_mem = state.EX.wrt_mem ;
                        // don't care about wrt_mem here
                    } else if (state.EX.wrt_mem == true){
                        // I type sw instruction
                        // mem[R[rs] + signExtImm] = R[rt]
                        newState.MEM.ALUresult = state.EX.Read_data1.to_ulong() + signextend(state.EX.Imm).to_ulong() ;
                        newState.MEM.Store_data = state.EX.Read_data2; // the value of rt, to be stored in memory
                        newState.MEM.Rs = state.EX.Rs ;
                        newState.MEM.Rt = state.EX.Rt ;
                        newState.MEM.Wrt_reg_addr.reset() ; // sw doesn't update the RF
                        newState.MEM.wrt_enable = state.EX.wrt_enable;  // sw doesn't write values - just takes previous 0
                        newState.MEM.wrt_mem = state.EX.wrt_mem;
                        newState.MEM.rd_mem = state.EX.rd_mem ;
                        // don't care about rd_mem here
                    }
                }
            } else if(state.EX.alu_op == false) {
                // r type subu instruction
                newState.MEM.ALUresult = state.EX.Read_data1.to_ulong() - state.EX.Read_data2.to_ulong() ;
                newState.MEM.Store_data.reset(); // don't care about this value
                newState.MEM.Rs = state.EX.Rs ;
                newState.MEM.Rt = state.EX.Rt ;
                newState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr ;
                newState.MEM.wrt_enable = state.EX.wrt_enable;
                newState.MEM.rd_mem = state.EX.rd_mem ;
                newState.MEM.wrt_mem = state.EX.wrt_mem ;
                // don't care about rd_mem and wrt_mem
            }
        }

        /* --------------------- ID stage --------------------- */
        if (state.ID.nop == 1){
            newState.EX.nop = state.ID.nop;
        } else {
            newState.EX.nop = state.ID.nop;
            string currentInstruction = state.ID.Instr.to_string();
            bitset<6> opcode = bitset<6>(currentInstruction.substr(0,6));
            
            if(opcode == 0){
                // R TYPE INSTRUCTION - ADDU AND SUBU
                newState.EX.Rs = bitset<5>(currentInstruction.substr(6,5));
                newState.EX.Rt = bitset<5>(currentInstruction.substr(11,5));
                newState.EX.Wrt_reg_addr = bitset<5>(currentInstruction.substr(16,5));
                newState.EX.Imm = bitset<16>(currentInstruction.substr(16,16)); // don't care Imm for R type
                
                bitset<3> functionField = bitset<3>(currentInstruction.substr(29,3));
                
                if(functionField == 3){
                    // this is for subu instruction
                    newState.EX.alu_op = false;
                } else {
                    // this is for addu instruction
                    newState.EX.alu_op = true;
                }
                newState.EX.wrt_enable = true;
                newState.EX.Read_data1 = myRF.readRF(newState.EX.Rs);
                newState.EX.Read_data2 = myRF.readRF(newState.EX.Rt);
                newState.EX.is_I_type = false;
                newState.EX.rd_mem = false;    //these are for I type lw and sw, don't care here
                newState.EX.wrt_mem = false; //these are for I type lw and sw, don't care here
            } else if (opcode == 0x23) {
                // I type instructions - lw
                // rs & signExtImm goes to ALU as input, rt is the destination register for lw
                // R[rt] = mem[R[rs] + signExtImm]
                
                newState.EX.Rs = bitset<5>(currentInstruction.substr(6,5));
                newState.EX.Rt = bitset<5>(currentInstruction.substr(11,5));
                newState.EX.Imm = bitset<16>(currentInstruction.substr(16,16));
                
                newState.EX.Wrt_reg_addr = newState.EX.Rt;
                
                newState.EX.alu_op = true; // since alu_op for lw is addition
                newState.EX.wrt_enable = true;
                newState.EX.Read_data1 = myRF.readRF(newState.EX.Rs);
                newState.EX.Read_data2.reset(); // don't care here
                // newState.EX.Read_data2 = myRF.readRF(newState.EX.Rt); lw needs only one value
                newState.EX.is_I_type = true;
                newState.EX.rd_mem = true;
                newState.EX.wrt_mem = false; // don't care here
            } else if (opcode == 0x2B){
                // I type instructions - sw
                // rs & signExtImm goes to ALU as input, rt is the value to be written in memory
                // mem[R[rs] + signExtImm] = R[rt]
                
                newState.EX.Rs = bitset<5>(currentInstruction.substr(6,5));
                newState.EX.Rt = bitset<5>(currentInstruction.substr(11,5));
                newState.EX.Imm = bitset<16>(currentInstruction.substr(16,16));
                
                newState.EX.Wrt_reg_addr.reset(); // don't care here
                newState.EX.alu_op = true;
                newState.EX.wrt_enable = false;
                newState.EX.Read_data1 = myRF.readRF(newState.EX.Rs);
                newState.EX.Read_data2 = myRF.readRF(newState.EX.Rt);
                newState.EX.is_I_type = true;
                newState.EX.wrt_mem = true;
                newState.EX.rd_mem = false; // don't care here
            }
            
            if(state.EX.rd_mem){
                if(newState.EX.Rt != 0){
                    if(newState.EX.Rt == state.EX.Wrt_reg_addr){
                        stall = 1;
                    }
                }
                if(newState.EX.Rs != 0){
                    if(newState.EX.Rs == state.EX.Wrt_reg_addr){
                        stall = 1;
                    }
                }
            }
            
        }


        
        /* --------------------- IF stage --------------------- */
        if (state.IF.nop == 1){
            // do nothing here, but forward the nop
            newState.ID.nop = state.IF.nop ;
        } else {
            newState.ID.nop = state.IF.nop ;
            newState.ID.Instr = myInsMem.readInstr(state.IF.PC);
            bitset<6> ifopcode = bitset<6>(newState.ID.Instr.to_string().substr(0,6));
            if(ifopcode == 0xFF){
                // HALT INSTRUCTION
                newState.IF.nop = true;
                newState.ID.nop = newState.IF.nop ;
            } else {
                newState.IF.PC = state.IF.PC.to_ulong() + 4;
            }
            
            if(stall){
                newState.IF = state.IF;
                newState.ID = state.ID;
                newState.EX.nop = true;
                stall = 0;
            }
            
        }


             
        if (state.IF.nop && state.ID.nop && state.EX.nop && state.MEM.nop && state.WB.nop)
            break;
        
        printState(newState, cycle); //print states after executing cycle 0, cycle 1, cycle 2 ...
        cycle++ ;
       
        state = newState; /*The end of the cycle and updates the current state with the values calculated in this cycle */ 
                	
    }
    
    myRF.outputRF(); // dump RF;	
	myDataMem.outputDataMem(); // dump data mem 
	
	return 0;
}
