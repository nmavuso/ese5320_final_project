-- ==============================================================
-- Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
-- Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
-- ==============================================================
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity main_top is
    generic (
        RESET_ACTIVE_LOW :  integer := 1
    );
    port (
        aresetn : in  std_logic;
        aclk : in  std_logic;
        ap_return : out  std_logic_vector(32 - 1 downto 0)
    );

-- attributes begin
-- attributes end
end entity;

architecture behav of main_top is
    component main is
        port (
            ap_rst_n : in  std_logic;
            ap_clk : in  std_logic;
            ap_return : out  std_logic_vector(32 - 1 downto 0)
        );
    end component;

    component main_ap_rst_n_if is
        generic (
            RESET_ACTIVE_LOW :  integer);
        port (
            dout :  out std_logic;
            din :  in std_logic);
    end component;

    signal sig_main_ap_rst_n :  std_logic;

begin
    main_U  : component main
        port map (
            ap_rst_n => sig_main_ap_rst_n,
            ap_clk => aclk,
            ap_return => ap_return
        );

    ap_rst_n_if_U : component main_ap_rst_n_if
        generic map (
            RESET_ACTIVE_LOW => RESET_ACTIVE_LOW)
        port map (
            dout => sig_main_ap_rst_n,
            din => aresetn);

end architecture;

