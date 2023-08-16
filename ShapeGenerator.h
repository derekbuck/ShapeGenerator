#ifndef SHAPEGENERATOR_H
#define SHAPEGENERATOR_H

#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>

namespace gen{

    //namespace variables
    extern int printBedWidth;
    extern int printBedLength;
    extern int printBedHeight;
    extern int headTemp;
    extern int bedTemp;
    extern double filamentWidth;


    extern double layerHeight;
    extern double lineWidth;
    extern double infill;
    extern double retractionDistance;
    extern int wallLineCount;
    extern int bottomLayers;
    extern int topLayers;
    extern int speed;
    extern int notPrintingSpeed;
    extern double fillSpeed;
    extern double innerWallSpeed;
    extern double outerWallSpeed;
    extern double topSpeed;

    extern double x;
    extern double y;
    extern double z;
    extern double e;
    extern double f;
    extern double xOld;
    extern double yOld;
    extern double zOld;
    extern double fOld;

    extern std::ofstream fout;

    struct Coordinate{ double x, y; };
    struct Line{ double m, b; };

    //namespace functions
    void Setup();
    void Finish();
    void Generate();
    void G0();
    void G1();
    void G0retraction();
    Line ParallelShift(double d, const Line& line);
    Line LineFromPoints(const Coordinate& point1, const Coordinate& point2);
    Coordinate Intersection(const Line& line1, const Line& line2);



    class Shape{

        //variables
        public:

            std::vector <Coordinate> baseVertice;
            std::vector <Coordinate> topVertice;
            std::vector <Coordinate> vertice;

            double height_;
            double infill_;
            int wallLineCount_;
            int bottomLayers_;
            int topLayers_;
            double fillSpeed_;
            double innerWallSpeed_;
            double outerWallSpeed_;
            double topSpeed_;

        private:


        //functions
        public:
            Shape();
            void NewBaseVertice(double x, double y);
            void NewTopVertice(double x, double y);
            void OutputLayer(int layer);
            void CleanVertices();
            void Wall();

        private:

    };

    extern std::vector <Shape> shape;


}



#endif // SHAPEGENERATOR_H
