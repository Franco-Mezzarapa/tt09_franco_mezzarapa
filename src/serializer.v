module serializer #(parameter MSG_SIZE = 64) (
    input wire [MSG_SIZE - 1: 0] iData_in,      
    input wire [$clog2(MSG_SIZE):0] iCounter,   
    input wire clk,                             
    input wire ena,                            
    input wire rst_n,                           
    
    output reg oData_flag,                     
    output reg oData_out                       
);

    integer serial_counter;                     
    reg done_serializing;

    always @(posedge clk) begin
        if (!rst_n) begin  // Synchronous reset instead of mixed reset
            oData_out <= 0;
            oData_flag <= 0;
            serial_counter <= MSG_SIZE - 1;
            done_serializing <= 0;
        end 
        else if (ena && iCounter == MSG_SIZE && !done_serializing) begin  
            oData_flag <= 1'b1;                  
            if (serial_counter >= 0) begin
                oData_out <= iData_in[serial_counter]; 
                serial_counter <= serial_counter - 1;  
            end 
            else if (serial_counter == -1) begin
                oData_out <= 0;                        
                oData_flag <= 1'b0;
                done_serializing <= 1;
            end
        end
    end
endmodule