./bin/tonemap --no-srgb -o out/tm_night_linear_e0.png in/tm_night.hdr
./bin/tonemap -o out/tm_night_srgb_e0.png in/tm_night.hdr
./bin/tonemap --filmic -o out/tm_night_filmic_e0.png in/tm_night.hdr
./bin/tonemap --filmic -e 2 -o out/tm_night_filmic_ep2.png in/tm_night.hdr

./bin/compose -o out/comp_gridcheck.png in/comp_grid.png in/comp_check.png
./bin/compose -o out/comp_gridramp.png in/comp_grid.png in/comp_ramp.png
./bin/compose -o out/comp_gridrampcheck.png in/comp_grid.png in/comp_ramp.png in/comp_check.png
./bin/compose -o out/comp_seaman.png in/comp_sea.png in/comp_man.png
