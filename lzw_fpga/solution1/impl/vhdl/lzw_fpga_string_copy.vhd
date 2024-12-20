-- ==============================================================
-- RTL generated by Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
-- Version: 2020.2
-- Copyright (C) Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
-- 
-- ===========================================================

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity lzw_fpga_string_copy is
port (
    ap_clk : IN STD_LOGIC;
    ap_rst : IN STD_LOGIC;
    ap_start : IN STD_LOGIC;
    ap_done : OUT STD_LOGIC;
    ap_idle : OUT STD_LOGIC;
    ap_ready : OUT STD_LOGIC;
    dest_address0 : OUT STD_LOGIC_VECTOR (9 downto 0);
    dest_ce0 : OUT STD_LOGIC;
    dest_we0 : OUT STD_LOGIC;
    dest_d0 : OUT STD_LOGIC_VECTOR (7 downto 0);
    src_address0 : OUT STD_LOGIC_VECTOR (9 downto 0);
    src_ce0 : OUT STD_LOGIC;
    src_q0 : IN STD_LOGIC_VECTOR (7 downto 0) );
end;


architecture behav of lzw_fpga_string_copy is 
    constant ap_const_logic_1 : STD_LOGIC := '1';
    constant ap_const_logic_0 : STD_LOGIC := '0';
    constant ap_ST_fsm_state1 : STD_LOGIC_VECTOR (3 downto 0) := "0001";
    constant ap_ST_fsm_state2 : STD_LOGIC_VECTOR (3 downto 0) := "0010";
    constant ap_ST_fsm_state3 : STD_LOGIC_VECTOR (3 downto 0) := "0100";
    constant ap_ST_fsm_state4 : STD_LOGIC_VECTOR (3 downto 0) := "1000";
    constant ap_const_lv32_0 : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000000000000";
    constant ap_const_lv32_1 : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000000000001";
    constant ap_const_lv1_0 : STD_LOGIC_VECTOR (0 downto 0) := "0";
    constant ap_const_lv32_2 : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000000000010";
    constant ap_const_lv10_0 : STD_LOGIC_VECTOR (9 downto 0) := "0000000000";
    constant ap_const_lv32_3 : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000000000011";
    constant ap_const_lv8_0 : STD_LOGIC_VECTOR (7 downto 0) := "00000000";
    constant ap_const_lv10_3FF : STD_LOGIC_VECTOR (9 downto 0) := "1111111111";
    constant ap_const_lv10_1 : STD_LOGIC_VECTOR (9 downto 0) := "0000000001";
    constant ap_const_boolean_1 : BOOLEAN := true;

attribute shreg_extract : string;
    signal ap_CS_fsm : STD_LOGIC_VECTOR (3 downto 0) := "0001";
    attribute fsm_encoding : string;
    attribute fsm_encoding of ap_CS_fsm : signal is "none";
    signal ap_CS_fsm_state1 : STD_LOGIC;
    attribute fsm_encoding of ap_CS_fsm_state1 : signal is "none";
    signal i_cast_fu_83_p1 : STD_LOGIC_VECTOR (63 downto 0);
    signal i_cast_reg_106 : STD_LOGIC_VECTOR (63 downto 0);
    signal ap_CS_fsm_state2 : STD_LOGIC;
    attribute fsm_encoding of ap_CS_fsm_state2 : signal is "none";
    signal icmp_ln8_fu_88_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal icmp_ln8_reg_112 : STD_LOGIC_VECTOR (0 downto 0);
    signal add_ln8_fu_94_p2 : STD_LOGIC_VECTOR (9 downto 0);
    signal add_ln8_reg_116 : STD_LOGIC_VECTOR (9 downto 0);
    signal i_reg_72 : STD_LOGIC_VECTOR (9 downto 0);
    signal ap_CS_fsm_state3 : STD_LOGIC;
    attribute fsm_encoding of ap_CS_fsm_state3 : signal is "none";
    signal icmp_ln8_2_fu_100_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal dest_addr_2_gep_fu_63_p3 : STD_LOGIC_VECTOR (9 downto 0);
    signal ap_CS_fsm_state4 : STD_LOGIC;
    attribute fsm_encoding of ap_CS_fsm_state4 : signal is "none";
    signal ap_NS_fsm : STD_LOGIC_VECTOR (3 downto 0);
    signal ap_ce_reg : STD_LOGIC;


begin




    ap_CS_fsm_assign_proc : process(ap_clk)
    begin
        if (ap_clk'event and ap_clk =  '1') then
            if (ap_rst = '1') then
                ap_CS_fsm <= ap_ST_fsm_state1;
            else
                ap_CS_fsm <= ap_NS_fsm;
            end if;
        end if;
    end process;


    i_reg_72_assign_proc : process (ap_clk)
    begin
        if (ap_clk'event and ap_clk = '1') then
            if (((ap_start = ap_const_logic_1) and (ap_const_logic_1 = ap_CS_fsm_state1))) then 
                i_reg_72 <= ap_const_lv10_0;
            elsif (((icmp_ln8_2_fu_100_p2 = ap_const_lv1_0) and (icmp_ln8_reg_112 = ap_const_lv1_0) and (ap_const_logic_1 = ap_CS_fsm_state3))) then 
                i_reg_72 <= add_ln8_reg_116;
            end if; 
        end if;
    end process;
    process (ap_clk)
    begin
        if (ap_clk'event and ap_clk = '1') then
            if ((ap_const_logic_1 = ap_CS_fsm_state2)) then
                add_ln8_reg_116 <= add_ln8_fu_94_p2;
                    i_cast_reg_106(9 downto 0) <= i_cast_fu_83_p1(9 downto 0);
                icmp_ln8_reg_112 <= icmp_ln8_fu_88_p2;
            end if;
        end if;
    end process;
    i_cast_reg_106(63 downto 10) <= "000000000000000000000000000000000000000000000000000000";

    ap_NS_fsm_assign_proc : process (ap_start, ap_CS_fsm, ap_CS_fsm_state1, icmp_ln8_reg_112, ap_CS_fsm_state3, icmp_ln8_2_fu_100_p2)
    begin
        case ap_CS_fsm is
            when ap_ST_fsm_state1 => 
                if (((ap_start = ap_const_logic_1) and (ap_const_logic_1 = ap_CS_fsm_state1))) then
                    ap_NS_fsm <= ap_ST_fsm_state2;
                else
                    ap_NS_fsm <= ap_ST_fsm_state1;
                end if;
            when ap_ST_fsm_state2 => 
                ap_NS_fsm <= ap_ST_fsm_state3;
            when ap_ST_fsm_state3 => 
                if (((icmp_ln8_2_fu_100_p2 = ap_const_lv1_0) and (icmp_ln8_reg_112 = ap_const_lv1_0) and (ap_const_logic_1 = ap_CS_fsm_state3))) then
                    ap_NS_fsm <= ap_ST_fsm_state2;
                else
                    ap_NS_fsm <= ap_ST_fsm_state4;
                end if;
            when ap_ST_fsm_state4 => 
                ap_NS_fsm <= ap_ST_fsm_state1;
            when others =>  
                ap_NS_fsm <= "XXXX";
        end case;
    end process;
    add_ln8_fu_94_p2 <= std_logic_vector(unsigned(i_reg_72) + unsigned(ap_const_lv10_1));
    ap_CS_fsm_state1 <= ap_CS_fsm(0);
    ap_CS_fsm_state2 <= ap_CS_fsm(1);
    ap_CS_fsm_state3 <= ap_CS_fsm(2);
    ap_CS_fsm_state4 <= ap_CS_fsm(3);

    ap_done_assign_proc : process(ap_start, ap_CS_fsm_state1, ap_CS_fsm_state4)
    begin
        if (((ap_const_logic_1 = ap_CS_fsm_state4) or ((ap_start = ap_const_logic_0) and (ap_const_logic_1 = ap_CS_fsm_state1)))) then 
            ap_done <= ap_const_logic_1;
        else 
            ap_done <= ap_const_logic_0;
        end if; 
    end process;


    ap_idle_assign_proc : process(ap_start, ap_CS_fsm_state1)
    begin
        if (((ap_start = ap_const_logic_0) and (ap_const_logic_1 = ap_CS_fsm_state1))) then 
            ap_idle <= ap_const_logic_1;
        else 
            ap_idle <= ap_const_logic_0;
        end if; 
    end process;


    ap_ready_assign_proc : process(ap_CS_fsm_state4)
    begin
        if ((ap_const_logic_1 = ap_CS_fsm_state4)) then 
            ap_ready <= ap_const_logic_1;
        else 
            ap_ready <= ap_const_logic_0;
        end if; 
    end process;

    dest_addr_2_gep_fu_63_p3 <= i_cast_reg_106(10 - 1 downto 0);

    dest_address0_assign_proc : process(i_cast_reg_106, ap_CS_fsm_state3, dest_addr_2_gep_fu_63_p3, ap_CS_fsm_state4)
    begin
        if ((ap_const_logic_1 = ap_CS_fsm_state4)) then 
            dest_address0 <= dest_addr_2_gep_fu_63_p3;
        elsif ((ap_const_logic_1 = ap_CS_fsm_state3)) then 
            dest_address0 <= i_cast_reg_106(10 - 1 downto 0);
        else 
            dest_address0 <= "XXXXXXXXXX";
        end if; 
    end process;


    dest_ce0_assign_proc : process(ap_CS_fsm_state3, ap_CS_fsm_state4)
    begin
        if (((ap_const_logic_1 = ap_CS_fsm_state4) or (ap_const_logic_1 = ap_CS_fsm_state3))) then 
            dest_ce0 <= ap_const_logic_1;
        else 
            dest_ce0 <= ap_const_logic_0;
        end if; 
    end process;


    dest_d0_assign_proc : process(src_q0, ap_CS_fsm_state3, ap_CS_fsm_state4)
    begin
        if ((ap_const_logic_1 = ap_CS_fsm_state4)) then 
            dest_d0 <= ap_const_lv8_0;
        elsif ((ap_const_logic_1 = ap_CS_fsm_state3)) then 
            dest_d0 <= src_q0;
        else 
            dest_d0 <= "XXXXXXXX";
        end if; 
    end process;


    dest_we0_assign_proc : process(icmp_ln8_reg_112, ap_CS_fsm_state3, icmp_ln8_2_fu_100_p2, ap_CS_fsm_state4)
    begin
        if (((ap_const_logic_1 = ap_CS_fsm_state4) or ((icmp_ln8_2_fu_100_p2 = ap_const_lv1_0) and (icmp_ln8_reg_112 = ap_const_lv1_0) and (ap_const_logic_1 = ap_CS_fsm_state3)))) then 
            dest_we0 <= ap_const_logic_1;
        else 
            dest_we0 <= ap_const_logic_0;
        end if; 
    end process;

    i_cast_fu_83_p1 <= std_logic_vector(IEEE.numeric_std.resize(unsigned(i_reg_72),64));
    icmp_ln8_2_fu_100_p2 <= "1" when (src_q0 = ap_const_lv8_0) else "0";
    icmp_ln8_fu_88_p2 <= "1" when (i_reg_72 = ap_const_lv10_3FF) else "0";
    src_address0 <= i_cast_fu_83_p1(10 - 1 downto 0);

    src_ce0_assign_proc : process(ap_CS_fsm_state2)
    begin
        if ((ap_const_logic_1 = ap_CS_fsm_state2)) then 
            src_ce0 <= ap_const_logic_1;
        else 
            src_ce0 <= ap_const_logic_0;
        end if; 
    end process;

end behav;
