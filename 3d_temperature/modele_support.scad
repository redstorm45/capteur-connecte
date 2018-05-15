
module vis(){
    color([0.5, 0.5, 0.5])
    difference(){
        union(){
            translate([0, 0, -17.5]) cylinder(h=20, r=1.2, $fn=100);
            cylinder(h=2.5, r=2.5, $fn=100);
        }
        translate([0, 0, 2.5]) cube(size=[1.5, 10, 2], center=true);
    }
}
module ecrou(largeur, hauteur, di){
    color([0.5, 0.5, 0.5])
    difference(){
        
    }
}
module support(){
    color([0.93, 0.69, 0.2])
    difference(){
        translate([-20, -3, 0]) cube(size=[40, 6, 15]);
        translate([-4, -4, -1]) cube(size=[8, 8, 4]);
        translate([-7.5, 0, -1]) cylinder(h=17, r=1.5, $fn=100);
        translate([-11.5, -4, 12]) cube(size=[23, 8, 5]);
        translate([7.5, 0, -1]) cylinder(h=17, r=1.5, $fn=100);
    }
}
module forme_cable(){
    union(){
        translate([5, 0, 0]) circle(r=2.3);
        translate([5, 2.3, 0]) square(size=4.6, center=true);
    }
}
module serre_cable(){
    difference(){
        cube(size=[24, 17, 7], center=true);
        difference(){
            translate([0, -2, 2]) rotate_extrude() forme_cable();
            translate([0, -9.5, 0]) cube(size=[20, 15, 20], center=true);
        }
        translate([0, -1.99, 2]) rotate(90, [1, 0, 0]) linear_extrude(height=10) forme_cable();
        mirror([1, 0, 0]) translate([0, -1.99, 2]) rotate(90, [1, 0, 0]) linear_extrude(height=10) forme_cable();
        translate([0, -2, -1]) cylinder(r=1.24, h=20);
    }
}

translate([0, -50, 0]) serre_cable($fn=30);
support();
translate([7.5, 0, 12]) vis();
translate([-7.5, 0, 12]) vis();

//support();
//serre_cable($fn=30);
