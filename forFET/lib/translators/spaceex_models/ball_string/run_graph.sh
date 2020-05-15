graph --bitmap-size "1600x1600" -T png -x -1.2 1 --pen-colors 1=#8888FF:2=#BBBBFF:3=red:4=black:5=#800c0 -C -B -f 0.04 -W 0 -m4 ballstring_segm_xv.gen -h 0.45 -X "position x" -Y "velocity v" > ballstring_segm_xv_pretty.png
#graph -T ai -x -1.2 1 --pen-colors 1=#8888FF:2=#BBBBFF:3=red:4=black:5=#800c0 -C -B -f 0.06 -W 0 -m2 ballstring_segm_xv.gen -h 0.71 -X "x" -Y "v" > ballstring_segm_xv.ai
#graph -T ai -x 0 30 --pen-colors 1=#8888FF:2=#BBBBFF:3=red:4=black:5=#800c0 -C -B -f 0.06 -W 0 -m2 out_tx8.txt -h 0.3 -X "t [s]" -Y "v\sbz\eb [ft/s]" > out_tx8_larger.ai
