rows = 8; 
total_length = 65+1;

rows = 8*2; 
total_length = 65*2 + 2;


thickness = 1.2; // max led-led distance 3.2mm
length = total_length - thickness ; 
height = 6; // can range from 5-10mm
side = length/rows;
lip = 1.2; // min lip is 1mm

frame = 9; // width of frame
frame_thickness = 3;

// Total size is 150 = frame*2 + total_length

module lines(trx, try) {
    translate([0,try,lip]) cube([length,thickness,height-lip]);
    translate([trx,0, 0]) cube([thickness,length,height]);
}

module base(){
    cube([length,thickness,height]);
    cube([thickness,length,height]);
    
    translate([0,side*(rows),0]) color("ForestGreen") cube([total_length,thickness,height]);
    translate([side*(rows),0,0]) color("ForestGreen") cube([thickness,total_length,height]);
    
    for(i= [0:rows-1]){
        lines(i*side, i*side);
    }
}

//base();
//rotate(a=[180,0,0]) translate([0,0,-height]) base();

module framed(){
    base();
    translate([-frame, -frame, height-frame_thickness]) cube([total_length + frame*2, frame, frame_thickness]);
    translate([-frame, total_length, height-frame_thickness]) cube([total_length + frame*2, frame, frame_thickness]);


    translate([-frame, -frame, height-frame_thickness]) cube([frame, total_length + frame*2, frame_thickness]);

    translate([total_length, -frame, height-frame_thickness]) cube([frame, total_length + frame*2, frame_thickness]);
}


//framed();
rotate(a=[180,0,0]) translate([frame,-total_length-frame,-height]) framed();
