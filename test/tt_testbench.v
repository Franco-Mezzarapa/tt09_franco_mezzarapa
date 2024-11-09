`timescale 1ns / 1ps

module tt_testbench();

    // Parameters
    localparam MSG_SIZE = 64;
    localparam KEY_SIZE = 8;
    localparam DEBUG_SIZE = 30; // Define the size of the debug shift register

    // Signals
    reg clk;
    reg rst_n;
    reg ena;
    reg [7:0] ui_in;
    reg [7:0] uio_in;
    wire [7:0] uo_out;
    wire [7:0] uio_out;
    wire [7:0] uio_oe;

    // Control signals
    reg [KEY_SIZE - 1:0] key;               // 8-bit key
    reg [MSG_SIZE - 1:0] message;           // 64-bit message
    reg [MSG_SIZE - 1:0] rebuilt_ciphertext; // Captured ciphertext from the module's output
    reg [MSG_SIZE - 1:0] ciphertext;        // Expected ciphertext calculated locally

    reg [DEBUG_SIZE - 1:0] rebuilt_debug;   // 24-bit debug shift register
    integer reset_counter;
    integer i;

    // Clock generation (10 MHz)
    localparam CLOCK_PERIOD = 100; // 10 MHz -> T = 1/10MHz = 100 ns
    initial clk = 0;
    always #(CLOCK_PERIOD / 2) clk = ~clk; // Toggle clock every half period

    // Instantiate the top module
    tt_um_franco_mezzarapa uut (
        .clk(clk),                    // Clock
        .ena(ena),                    // Enable
        .rst_n(rst_n),                // Reset (active low)
        .ui_in(ui_in),                // Input array (0 is serial line, 1 is key, 2 is msg)
        .uo_out(uo_out),              // Output array (0 is serial line, 1 is out stat, 2 is encrypt stat.)
        .uio_in(uio_in),
        .uio_out(uio_out),
        .uio_oe(uio_oe)
    );

    // Initial setup
    initial begin
        // Initialize signals
        clk = 0;
        rst_n = 1;                    // Reset is active low, so 1 means reset is inactive here
        ena = 0;
        ui_in = 8'b00000000;
        uio_in = 8'b00000000;
        rebuilt_debug = 0;
        
        key = 8'hA5;                  // Example 8-bit key
        message = 64'hA3B1F9D2E7C6A594; // Example 64-bit message
        
    // Enable and perform initial reset
    ena = 1;
    rst_n = 0; // Assert reset
    #CLOCK_PERIOD;
    rst_n = 1; // Release reset
    #CLOCK_PERIOD;
    
    for (i = 0; i < 99; i = i +1) begin
        rst_n = 0; // Assert reset
        #CLOCK_PERIOD;
        rst_n = 1; // Release reset
    end
    
    
    // Load the key completely so that oBit_counter_key reaches 8
    ui_in[1] = 1; // Set key loading flag
    for (i = KEY_SIZE - 1; i >= 0; i = i - 1) begin
        ui_in[0] = key[i];
        #CLOCK_PERIOD;
    end
    ui_in[1] = 0;
    ui_in[0] = 0;
    
    // Wait for one more clock cycle to ensure conditions are met
    #CLOCK_PERIOD;
    
        // Setting up control inputs
        ui_in[3] = 0;
        ui_in[4] = 0;
        ui_in[5] = 0;
        ui_in[6] = 1; // Side channel mode.
        ui_in[7] = 1; // Another control signal, assuming it's needed
    
        
        
        // Load the key
        ui_in[1] = 1;
        for (i = KEY_SIZE - 1; i >= 0; i = i - 1) begin
            ui_in[0] = key[i];
            #CLOCK_PERIOD;
        end
        ui_in[1] = 0;
        ui_in[0] = 0;
        
        // Load the message
        #(CLOCK_PERIOD * 5);
        ui_in[2] = 1;
        for (i = MSG_SIZE - 1; i >= 0; i = i - 1) begin
            ui_in[0] = message[i];
            #CLOCK_PERIOD;
        end
        ui_in[2] = 0;
        ui_in[0] = 0;

        // Wait until ciphertext output is ready (assuming uo_out[1] as a flag)
        wait(uo_out[1]);
        for (i = MSG_SIZE - 1; i >= 0; i = i - 1) begin
            @(posedge clk);
            rebuilt_ciphertext[i] = uo_out[0];
        end
        
        // Capture 24-bit debug output serially from uo_out[7]
        for (i = DEBUG_SIZE - 1; i >= 0; i = i - 1) begin
            @(posedge clk);
            rebuilt_debug[i] = uo_out[7];
        end

        // Perform XOR operation on each 8-bit chunk of the message with the key
        for (i = 0; i < MSG_SIZE / KEY_SIZE; i = i + 1) begin
            ciphertext[i*KEY_SIZE +: KEY_SIZE] = message[i*KEY_SIZE +: KEY_SIZE] ^ key;
        end

        // Display the results
        $display("Key:                 %h", key);
        $display("Message:             %h", message);
        $display("Computed Ciphertext: %h", ciphertext);
        $display("Rebuilt Ciphertext:  %h", rebuilt_ciphertext);
        $display("Debug Output (24 bits): %h", rebuilt_debug);

        // Compare computed ciphertext with rebuilt_ciphertext
        if (ciphertext == rebuilt_ciphertext) begin
            $display("Test Passed: Ciphertext matches rebuilt_ciphertext.");
        end else if (64'h0f1d557e4b6a0938 == rebuilt_ciphertext) begin
            $display("Test Passed: Ciphertext matches XOR with key AC - ui_in[3] - Always Active.");
        end else if (64'h07f6d250e3b1a7948 == rebuilt_ciphertext) begin
            $display("Test Passed: Ciphertext matches XOR with key AC - ui_in[4] - Reset Active.");
        end else if (message == rebuilt_ciphertext) begin
            $display("Test Passed: Ciphertext matches message (no encryption) - ui_in[5] - No key.");
        end else begin
            $display("Test Failed: Ciphertext does not match any expected result.");
        end

        $finish; // End the simulation
    end

endmodule
