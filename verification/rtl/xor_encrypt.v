module xor_encrypt #(parameter MSG_SIZE = 64, KEY_SIZE = 8)(
    input wire clk,       
    input wire rst_n,       
    input wire ena,
    
    input wire [MSG_SIZE - 1:0] iMessage,    // Input message (plaintext)
    input wire [KEY_SIZE - 1:0] iKey,         // XOR key
    
    input wire [$clog2(MSG_SIZE) :0] iMessage_bit_counter,
    input wire [$clog2(KEY_SIZE)  :0] iKey_bit_counter,
    
    output reg encryption_status,
    output reg [$clog2(MSG_SIZE):0] oCiphertext_counter,    // 8-bit chunk counter
    output reg [MSG_SIZE - 1 :0] oCiphertext                   // Ciphertext output
);


always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        //oCiphertext_counter <= 0;                    // Reset counter to 0
        oCiphertext <= 64'b0;                        // Initialize ciphertext to zero
        encryption_status <= 1'b0;                   // Reset encryption status
        oCiphertext_counter <= 0;
    end else if (ena && (iMessage_bit_counter == MSG_SIZE) && (iKey_bit_counter == KEY_SIZE)) begin
    
        encryption_status <= 1;
        
       if (oCiphertext_counter < (MSG_SIZE / KEY_SIZE)) begin
                
            // XOR each 8 bits of the message with the key
            oCiphertext[oCiphertext_counter * KEY_SIZE +: KEY_SIZE] <= iMessage[oCiphertext_counter * KEY_SIZE +: KEY_SIZE] ^ iKey;
            oCiphertext_counter <= oCiphertext_counter + 1;   // Increment the counter
       end else if (oCiphertext_counter == (MSG_SIZE / KEY_SIZE)) begin
               encryption_status <= 0; 
        end
    end  
end

endmodule
