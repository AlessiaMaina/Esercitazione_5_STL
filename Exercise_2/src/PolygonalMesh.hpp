#pragma once

#include <iostream>
#include <Eigen/Eigen>

using namespace std;
using namespace Eigen;

namespace PolygonalLibrary{

struct PolygonalMesh
{
    // DEFINITION of Cell0D
    unsigned int number_Cell0Ds = 0;                                        // Number of Cell0Ds
    vector<unsigned int> id_Cell0Ds = {};                                   // Id of Cell0Ds
    vector<Vector2d> coordinates_Cell0Ds = {};                              // Coordinates X and Y of Cell0Ds
    map<unsigned int, list<unsigned int>> markers_Cell0Ds = {};             // Marker of Cell0Ds: map<marker, list id>

    // DEFINITION of Cell1D
    unsigned int number_Cell1Ds = 0;                                        // Number of Cell1Ds
    vector<unsigned int> id_Cell1Ds = {};                                   // Id of Cell1Ds
    vector<Vector2i> vertices_Cell1Ds = {};                                 // Vertices from Origin to End of Cell1Ds
    map<unsigned int, list<unsigned int>> markers_Cell1Ds = {};             // Marker of Cell1Ds: map<marker, list id>

    // DEFINITION of Cell2D
    unsigned int number_Cell2Ds = 0;                                        // Number of Cell2Ds
    vector<unsigned int> id_Cell2Ds = {};                                   // Id of Cell2Ds
    vector<vector<unsigned int>> vertices_Cell2Ds = {};                     // Vertices of Cell2Ds
    vector<vector<unsigned int>> edges_Cell2Ds = {};                        // Edges of Cell2Ds
    // Markers of Cell2Ds are all ZEROS: no map for markers

};

}
