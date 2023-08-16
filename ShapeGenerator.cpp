#include "ShapeGenerator.h"

int gen::printBedWidth = 200;
int gen::printBedLength = 200;
int gen::printBedHeight = 250;
int gen::headTemp = 200;
int gen::bedTemp = 60;
double gen::filamentWidth = 1.75;

double gen::layerHeight = .2;
double gen::lineWidth = .4;
double gen::infill = 20;
double gen::retractionDistance = 6;
int gen::wallLineCount = 3;
int gen::bottomLayers = 4;
int gen::topLayers = 4;
int gen::speed;
int gen::notPrintingSpeed = 75;
double gen::fillSpeed = 25;
double gen::innerWallSpeed = 25;
double gen::outerWallSpeed = 25;
double gen::topSpeed = 25;

double gen::x;
double gen::y;
double gen::z;
double gen::e = 0;
double gen::f;
double gen::xOld;
double gen::yOld;
double gen::zOld;
double gen::fOld;

std::ofstream gen::fout;

std::vector <gen::Shape> gen::shape;

void gen::Generate(){

    Setup();

    double maxHeight = 0;
    for (auto &i: shape)
        if (i.height_ > maxHeight)
            maxHeight = i.height_;
    if (maxHeight > printBedHeight){
        std::cout << "You are trying to print an object too large for your print bed." << std::endl;
        return;
    }
    int totalLayers = maxHeight / layerHeight;

    for (int layer = 1; layer <= totalLayers; layer++){
        std::cout << "Layer: " << layer << std::endl;
        for (auto &i: shape){
            //if (i.height_ <= layer / layerHeight){
                i.OutputLayer(layer);
            //}
        }
    }

    Finish();
}

void gen::Setup(){

    fout.open("file_name.gcode");

    //delete eventually
    //GCODE setup commands
    fout << "; Setup Commands" << std::endl;
    fout << "M140 S" << bedTemp << std::endl;
    fout << "M105" << std::endl;
    fout << "M104 S" << headTemp << std::endl;
    fout << "M105" << std::endl;
    fout << "M109 S" << headTemp << std::endl;
    fout << "M82 ; absolute extrusion mode" << std::endl;
    fout << "G92 E0 ; Reset Extruder" << std::endl;
    fout << "G28 ; Home all axes" << std::endl;
    fout << "G1 Z2.0 F3000 ; Move Z Axis up little to prevent scratching of Heat Bed" << std::endl;
    fout << "G1 X0.1 Y10 Z0.3 F5000.0 ; Move to start position" << std::endl;
    fout << "G1 X0.1 Y190.0 Z0.3 F1500.0 E15 ; Draw the first line" << std::endl;
    fout << "G1 X0.4 Y190.0 Z0.3 F5000.0 ; Move to side a little" << std::endl;
    fout << "G1 X0.4 Y10 Z0.3 F1500.0 E30 ; Draw the second line" << std::endl;
    fout << "G92 E0 ; Reset Extruder" << std::endl;
    fout << "G1 Z2.0 F3000 ; Move Z Axis up little to prevent scratching of Heat Bed" << std::endl;
    fout << "G1 X5 Y20 Z0.3 F5000.0; Move over to prevent blob squish" << std::endl;
    fout << "G0 F9000 Z2" << std::endl;
    fout << "M300 S800 P500" << std::endl;

    fout << "G0 F1200" << std::endl;


}

void gen::Finish(){

    fout << std::endl << ";-----------------------------------" << std::endl;
    fout << "; Finishing Commands" << std::endl;
    fout << "M300 S440 P200" << std::endl;
    fout << "M140 S0" << std::endl;
    fout << "M107" << std::endl;
    fout << "G91 ;Relative positioning" << std::endl;
    fout << "G1 E-2 F2700 ;Retract a bit" << std::endl;
    fout << "G1 E-2 Z0.2 F2400 ;Retract and raise Z" << std::endl;
    fout << "G1 X5 Y5 F3000 ;Wipe out" << std::endl;
    fout << "G1 Z50 ;Raise Z more" << std::endl;
    fout << "G90 ;Absolute positionning" << std::endl;
    fout << "G1 X0 Y220 ;Present print" << std::endl;
    fout << "M106 S0 ;Turn-off fan" << std::endl;
    fout << "M104 S0 ;Turn-off hotend" << std::endl;
    fout << "M140 S0 ;Turn-off bed" << std::endl;
    fout << "M84 X Y Z E ;Disable all steppers" << std::endl;
    fout << "M82 ;absolute extrusion mode" << std::endl;
    fout << "M104 S0" << std::endl;
    fout << "; " << e / 1000 << "m of filament used." << std::endl;

}

void gen::G0(){

    //if (std::isnan(x) || std::isnan(y)) return;
    f = notPrintingSpeed * 60;
    if (xOld == x && yOld == y && zOld == z && fOld == f) return;

    fout << "G0";
    if (f != fOld) fout << " F" << f;
    fout << " X" << x;
    fout << " Y" << y;
    if (z != zOld) fout << " Z" << z;
    fout << std::endl;

    xOld = x;
    yOld = y;
    zOld = z;
    fOld = f;

}

void gen::G1(){

    f = speed * 60;
    //if (std::isnan(x) || std::isnan(y)) return;
    if (xOld == x && yOld == y && zOld == z && fOld == f) return;

    e += (hypot(x - xOld, y - yOld) * layerHeight * lineWidth) / (M_PI * (1.75 / 2) * (1.75 / 2));
    //e += sqrt(((x - xOld) * (x - xOld)) + ((y - yOld) * (y - yOld))) * ((lineWidth * layerHeight) / 2.3);

    fout << "G1";
    if (f != fOld) fout << " F" << f;
    fout << " X" << x;
    fout << " Y" << y;
    if (z != zOld) fout << " Z" << z;
    fout << " E" << e;
    fout << std::endl;

    xOld = x;
    yOld = y;
    zOld = z;
    fOld = f;

}

void gen::G0retraction(){

    f = notPrintingSpeed * 60;
    e -= retractionDistance;
    fout << "G1";
    if (f != fOld) fout << " F" << f;
    fout << " E" << e << std::endl;

    G0();

    e += retractionDistance;
    fout << "G1 E" << e << std::endl;

    fOld = f;

}

gen::Shape::Shape(){

    infill_ = infill;
    wallLineCount_ = wallLineCount;
    bottomLayers_ = bottomLayers;
    topLayers_ = topLayers;
    fillSpeed_ = fillSpeed;
    innerWallSpeed_ = innerWallSpeed;
    outerWallSpeed_ = outerWallSpeed;
    topSpeed_ = topSpeed;

}

//adds new vertice only if it is not a duplicate of the previous vertice
void gen::Shape::NewBaseVertice(double x, double y){
    if(baseVertice.size())
        if (abs(baseVertice.back().x - x) < .01 && abs(baseVertice.back().y - y) < .01)
            return;
    baseVertice.resize(baseVertice.size() + 1);
    baseVertice.back().x = x;
    baseVertice.back().y = y;
}

//adds new vertice only if it is not a duplicate of the previous vertice
void gen::Shape::NewTopVertice(double x, double y){
    if(topVertice.size())
        if (abs(topVertice.back().x - x) < .01 && abs(topVertice.back().y - y) < .01)
            return;
    topVertice.resize(topVertice.size() + 1);
    topVertice.back().x = x;
    topVertice.back().y = y;
}

//deletes and vertices that do not change slope from prior and next vertice
//then, adds .000001 to any undefined slopes to prevent NaN
void gen::Shape::CleanVertices(){

    int loops = vertice.size();
    for ( int i = 0; i < vertice.size(); i++){
        double slope1, slope2;
        if (!i){
            slope1 = (vertice.back().y - vertice[0].y) / (vertice.back().x - vertice[0].x);
            slope2 = (vertice[1].y - vertice[0].y) / (vertice[1].x - vertice[0].x);
        }
        else if (i == vertice.size() - 1){
            slope1 = (vertice.back().y - vertice[0].y) / (vertice.back().x - vertice[0].x);
            slope2 = (vertice.back().y - vertice[i-1].y) / (vertice.back().x - vertice[i-1].x);
        }
        else{
            slope1 = (vertice[i].y - vertice[i-1].y) / (vertice[i].x - vertice[i-1].x);
            slope2 = (vertice[i].y - vertice[i+1].y) / (vertice[i].x - vertice[i+1].x);
        }
        //std::cout << i << "   " << slope1 << "   " << slope2 << std::endl;
        //if (abs(slope1 - slope2) < .0001) {
        if(slope1 == slope2){
            vertice.erase(vertice.begin() + i);
            //std::cout <<"delete" ;
            i--;
        }
        if (std::isinf(slope1) && std::isinf(slope2)){
            vertice.erase(vertice.begin() + i);
            //std::cout <<"delete" ;
            i--;
        }
        //if (std::isnan(slope1) && std::isnan(slope2)){
        //    vertice.erase(vertice.begin() + i);
        //    std::cout <<"delete" ;
        //    i -= 1;
        //}
        //std::cout << std::endl;
    }
    while(1){
        if (vertice[0].x == vertice.back().x) vertice[0].x += .000001;
        for (int i = 1; i < vertice.size() - 1; i++){
            if (vertice[i].x == vertice[i+1].x) vertice[i+1].x += .000001;
        }
        if (vertice[0].x != vertice.back().x) break;
    }
    //for (int i = 0; i < vertice.size(); i++) std::cout << i << " " << vertice[i].x << std::endl;
}

void gen::Shape::OutputLayer(int layer){

    if (layer > height_ / layerHeight) return;

    z = layer * layerHeight;
    G0();

    std::vector <double> xChange, yChange;
    xChange.resize(topVertice.size());
    yChange.resize(topVertice.size());
    for(int i = 0; i < baseVertice.size(); i++){
        xChange[i] = topVertice[i].x - baseVertice[i].x;
        yChange[i] = topVertice[i].y - baseVertice[i].y;
    }
    double moveFactor = ((height_ / layerHeight) - ((height_ / layerHeight) - layer + 1)) / ((height_ / layerHeight) - 1);

    vertice.resize(baseVertice.size());
    for (int i = 0; i < vertice.size(); i++){
        vertice[i].x = baseVertice[i].x + (xChange[i] * moveFactor);
        vertice[i].y = baseVertice[i].y + (yChange[i] * moveFactor);
    }

    CleanVertices();

    //for (auto &i : vertice){
    //    std::cout << i.x << "," << i.y << std::endl;
    //}

    Wall();

}

void gen::Shape::Wall(){

    std::vector <Line> line;
    line.resize(vertice.size());

    for (int j = 0; j < wallLineCount_; j++){

        if (!j) speed = outerWallSpeed_;
        else speed = innerWallSpeed_;

        for (int i = 0; i < vertice.size(); i++){

            //points of two vectors at this angle, 10-> and 12->
            Coordinate P[3];
            P[1].x = vertice[i].x;
            P[1].y = vertice[i].y;
            if(!i){
                P[0].x = vertice.back().x;
                P[0].y = vertice.back().y;
            }
            else{
                P[0].x = vertice[i-1].x;
                P[0].y = vertice[i-1].y;
            }
            if(i == vertice.size() - 1){
                P[2].x = vertice[0].x;
                P[2].y = vertice[0].y;
            }
            else{
                P[2].x = vertice[i+1].x;
                P[2].y = vertice[i+1].y;
            }

            //convert the two vectors from the current point to slope intercept form
            Line oldLine[2];
            oldLine[0] = LineFromPoints(P[1], P[0]);
            oldLine[1] = LineFromPoints(P[1], P[2]);

            //two possible lines used to determine new point
            Line lineChoice[2];
            lineChoice[0] = ParallelShift(lineWidth * j, oldLine[1]);
            lineChoice[1] = ParallelShift(-lineWidth * j, oldLine[1]);

            //calculate intercepts of two new lines and line one clockwise
            Coordinate intercept[2];
            intercept[0] = Intersection(lineChoice[0], oldLine[0]);
            intercept[1] = Intersection(lineChoice[1], oldLine[0]);

            //determine which line to use, depending on if angle is greater or less than 180, because...
            //if it is less than 180, we want the first parallel line we cross, greater than 180, visa versa
            double angle = atan2(P[0].y - P[1].y, P[0].x - P[1].x) - atan2(P[2].y - P[1].y, P[2].x - P[1].x);
            angle = (angle >= 0) ? angle : angle + 2 * M_PI;
            double d0 = hypot(P[0].x - intercept[0].x, P[0].y - intercept[0].y);
            double d1 = hypot(P[0].x - intercept[1].x, P[0].y - intercept[1].y);
            if (angle <= M_PI) line[i] = (d0 <= d1) ? lineChoice[0] : lineChoice[1];
            else line[i] = (d0 > d1) ? lineChoice[0] : lineChoice[1];

        }

        //find intersections of new line and put them into passPoint vector
        std::vector <Coordinate> passPoint;
        passPoint.resize(vertice.size());
        for (int i = 0; i < line.size(); i++){
            if (i){
                passPoint[i].x = (line[i-1].b - line[i].b) / (line[i].m - line[i-1].m);
                passPoint[i].y = line[i].m * passPoint[i].x + line[i].b;
            }
            else {
                passPoint[i].x = (line.back().b - line[i].b) / (line[i].m - line.back().m);
                passPoint[i].y = line[i].m * passPoint[i].x + line[i].b;
            }
        }

        //G0 to fist point, for loop to go around shape, and then a final g1 to return to start.
        x = passPoint[0].x;
        y = passPoint[0].y;
        if (!j) G0retraction();
        else G0();
        for (int i = 1; i < passPoint.size(); i++){
            x = passPoint[i].x;
            y = passPoint[i].y;
            G1();
        }
        x = passPoint[0].x;
        y = passPoint[0].y;
        G1();
    }
}

gen::Line gen::ParallelShift(double d, const gen::Line& line){

    double x1 = 0;
    double x2 = 1;
    double y1 = line.m * x1 + line.b;
    double y2 = line.m * x2 + line.b;

    double r = hypot(x2-x1, y2-y1);
    double xShift = (d / r) * (y1 - y2);
    double yShift = (d / r) * (x2 - x1);
    double x3 = x1 + xShift;
    double y3 = y1 + yShift;
    double x4 = x2 + xShift;
    double y4 = y2 + yShift;

    Line shiftedLine;
    shiftedLine.m = (y4 - y3) / (x4 - x3);
    shiftedLine.b = y4 - shiftedLine.m * x4;

    return shiftedLine;

}

gen::Line gen::LineFromPoints(const gen::Coordinate& point1, const gen::Coordinate& point2){
    Line line;
    line.m = (point1.y - point2.y) / (point1.x - point2.x);
    line.b = point1.y - line.m * point1.x;
    return line;
}


gen::Coordinate gen::Intersection(const gen::Line& line1, const gen::Line& line2){
    Coordinate intercept;
    intercept.x = (line1.b - line2.b) / (line2.m - line1.m);
    intercept.y = line2.m * intercept.x + line2.b;
    return intercept;
}
