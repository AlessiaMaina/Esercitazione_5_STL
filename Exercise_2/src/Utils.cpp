#include "Utils.hpp"
#include "PolygonalMesh.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include <Eigen/Eigen>

namespace PolygonalLibrary
{

bool importMesh(const string& filePath,
                PolygonalMesh& mesh)
{
    if(!importCell0Ds(filePath + "/Cell0Ds.csv",             // First CSV file
                      mesh))
    {
        return false;
    }
    else
    {
        // TEST n°1 part 1 : "ALL MARKERS HAVE BEEN STORED CORRECTLY"
        cout << "Cell0D MARKER" << endl;
        for(auto it = mesh.markers_Cell0Ds.begin(); it != mesh.markers_Cell0Ds.end(); it++)
        {
            cout << "Marker KEY:\t" << it -> first << "\t Marker VALUES:";
            for(const unsigned int id : it -> second)
                cout << "\t" << id;
            cout << endl;
        }
        cout << "\n" << endl;
    }

    if(!importCell1Ds(filePath + "/Cell1Ds.csv",             // Second CSV file
                      mesh))
    {
        return false;
    }
    else
    {
        // TEST n°1 part 2 : "ALL MARKERS HAVE BEEN STORED CORRECTLY"
        cout << "Cell1D MARKER" << endl;
        for(auto it = mesh.markers_Cell1Ds.begin(); it != mesh.markers_Cell1Ds.end(); it++)
        {
            cout << "Marker KEY:\t" << it -> first << "\t Marker VALUES:";
            for(const unsigned int id : it -> second)
                cout << "\t" << id;
            cout << endl;
        }
        cout << "\n" << endl;
    }

    if(!importCell2Ds(filePath + "/Cell2Ds.csv",             // Third CSV file
                      mesh))
    {
        return false;
    }
    else
    {
        // TEST n°2 : "THE MESH HAS BEEN BUILD CORRECTLY"
        for(unsigned int c = 0; c < mesh.number_Cell2Ds; c++)
        {
            vector<unsigned int> Edg = mesh.edges_Cell2Ds[c];
            unsigned int n_Edg = Edg.size();         // n, as the mesh is POLYGONAL (n-edges)

            // For each edge, check if indices of Origin and End corresponds to indices of vertices
            for(unsigned int e = 0; e < n_Edg; e++)
            {
                const unsigned int Origin = mesh.vertices_Cell1Ds[Edg[e]][0];       // Origin vertex
                const unsigned int End = mesh.vertices_Cell1Ds[Edg[e]][1];          // End vertex

                auto findOrigin = find(mesh.vertices_Cell2Ds[c].begin(), mesh.vertices_Cell2Ds[c].end(), Origin);
                if(findOrigin == mesh.vertices_Cell2Ds[c].end())
                {
                    cerr << "ATTENTION: Wrong mesh in the begin!" << endl;     // 'Origin' is NOT in the vector
                    return 2;
                }

                auto findEnd = find(mesh.vertices_Cell2Ds[c].begin(), mesh.vertices_Cell2Ds[c].end(), End);
                if(findEnd == mesh.vertices_Cell2Ds[c].end())
                {
                    cerr << "ATTENTION: Wrong mesh in the end!" << endl;     // 'End' is NOT in the vector
                    return 3;
                }                

                // TEST n°3 : "POLYGON MUST HAVE AT LEAST 3 EDGES"
                if(mesh.edges_Cell2Ds[c].size() < 3)
                {
                    cerr << "ATTENTION: There are less than 3 edges, the figure is NOT a Polygon. This is IMPOSSIBLE!" << endl;
                    return 4;
                }

                // TEST n°4 : "NUM OF EDGES MUST BE EQUAL TO NUM OF VERTICES"
                if(mesh.edges_Cell2Ds[c].size() != mesh.vertices_Cell2Ds[c].size())
                {
                    cerr << "ATTENTION: Num of edges is NOT equal to num of vertices. This is IMPOSSIBLE!" << endl;
                    return 5;
                }

                // TEST n°5 : "THE EDGES HAVE NON-ZERO LENGTH"
                if(Origin == End)    // If 'origin' coincides with 'end', it means that there is NO distance between the 2 vertices
                {
                    cerr << "ATTENTION: There is an edge with 0 lenght. This is IMPOSSIBLE!" << endl;
                    return 6;
                }
            }

            // TEST n°6 : "THE AREA OF TRIANGLES IS NON-ZERO"
            vector<double> triangleArea;
            double polygonArea = 0;     // Polygon with "number edges > 3" and "number vertices > 3"

            vector<unsigned int> Vert = mesh.vertices_Cell2Ds[c];
            unsigned int n_Vert = Vert.size();     // n, as the mesh is POLYGONAL (n-vertices)
            unsigned int numTrianglesOfPolygon = n_Vert - 2;    // Indicates how many Triangles make up a n-sided Polygon (n > 3)
            // EXAMPLE: if a POLYGON has 7 EDGES, it will be made up of 5 TRIANGLES (in fact, 7-2 = 5)

            for(unsigned int v = 0; v < numTrianglesOfPolygon ; v++)
            {
                // Coordinates of the FIRST vertex (point A)
                double xA = mesh.coordinates_Cell0Ds[Vert[v]][0];
                double yA = mesh.coordinates_Cell0Ds[Vert[v]][1];

                // Coordinates of the SECOND vertex (point B)
                double xB = mesh.coordinates_Cell0Ds[Vert[v+1]][0];
                double yB = mesh.coordinates_Cell0Ds[Vert[v+1]][1];

                // Coordinates of the THIRD vertex (point C)
                double xC = mesh.coordinates_Cell0Ds[Vert[v+2]][0];
                double yC = mesh.coordinates_Cell0Ds[Vert[v+2]][1];

                MatrixXd vertMatr(3,3);     // Definition of a matrix with the coordinates of 3 vertices (A, B and C)
                vertMatr << xA, yA, 1,      // coord x of A      coord y of A     coord z of A (equal to 1)
                            xB, yB, 1,      // coord x of B      coord y of B     coord z of B (equal to 1)
                            xC, yC, 1;      // coord x of C      coord y of C     coord z of C (equal to 1)
                double det = vertMatr.determinant();
                triangleArea.push_back((1.0/2.0)*abs(det));     // FORMULA for the AREA OF GENERIC TRIANGLE with vertices

                double singleTriangle = triangleArea[0];        // Area for a Polygon made up of 1 TRIANGLE

                if(numTrianglesOfPolygon == 1)      // Case of a single TRIANGLE
                {
                    return singleTriangle;
                }
                else                                // Case of multiple triangles, POLYGON with num vertices > 3
                {
                    for(unsigned int i = 0; i < numTrianglesOfPolygon; i++)
                    {
                        polygonArea += triangleArea[i];         // Area of Polygon is equal to the sum of ALL TRIANGLES
                    }
                }

                double machineEpsilon = numeric_limits<double>::epsilon();
                double edgeTolerance = pow(10,-14);      // Tolerance chosen for the edge
                double areaTolerance = max((1.0/2.0)*pow(edgeTolerance, 2), machineEpsilon);     // Max between 2 tolerances

                if(singleTriangle <= areaTolerance)
                {
                    cerr << "ATTENTION: The area of the Triangle is 0. This is IMPOSSIBLE!" << endl;
                    return 7;
                }

                if(polygonArea <= areaTolerance)
                {
                    cerr << "ATTENTION: The area of the Polygon is 0. This is IMPOSSIBLE!" << endl;
                    return 8;
                }
            }
        }
    }
    return true;
}

// ******************************************************************************************************************************
bool importCell0Ds(const string& fileName,
                   PolygonalMesh& mesh)
{
    ifstream file;
    file.open(fileName);
    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();
    listLines.pop_front();

    mesh.number_Cell0Ds = listLines.size();
    if (mesh.number_Cell0Ds == 0)
    {
        cerr << "ATTENTION: There is no CELL 0D!" << endl;
        return false;
    }

    mesh.id_Cell0Ds.reserve(mesh.number_Cell0Ds);
    mesh.coordinates_Cell0Ds.reserve(mesh.number_Cell0Ds);

    for(const string& line : listLines)
    {
        istringstream convert(line);

        unsigned int Id;
        unsigned int Marker;
        Vector2d Coord;
        char del = ';';      // Definition of a delimiter

        convert >> Id >> del >> Marker >> del >> Coord(0) >> del >> Coord(1);
        // Coord(0) = X coordinate     Coord(1) = Y coordinate

        mesh.id_Cell0Ds.push_back(Id);
        mesh.coordinates_Cell0Ds.push_back(Coord);

        if(Marker != 0)
        {
            auto ret = mesh.markers_Cell0Ds.insert({Marker, {Id}});
            if(!ret.second)
                (ret.first)->second.push_back(Id);
        }
    }

    file.close();
    return true;
}

// ******************************************************************************************************************************
bool importCell1Ds(const string& fileName,
                   PolygonalMesh& mesh)
{
    ifstream file;
    file.open(fileName);
    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();
    listLines.pop_front();

    mesh.number_Cell1Ds = listLines.size();
    if (mesh.number_Cell1Ds == 0)
    {
        cerr << "ATTENTION: There is no CELL 1D!" << endl;
        return false;
    }

    mesh.id_Cell1Ds.reserve(mesh.number_Cell1Ds);
    mesh.vertices_Cell1Ds.reserve(mesh.number_Cell1Ds);

    for(const string& line : listLines)
    {
        istringstream convert(line);

        unsigned int Id;
        unsigned int Marker;
        Vector2i Vert;
        char del = ';';      // Definition of a delimiter

        convert >> Id >> del >> Marker >> del >> Vert(0) >> del >> Vert(1);
        // Vert(0) = Origin       Vert(1) = End

        mesh.id_Cell1Ds.push_back(Id);
        mesh.vertices_Cell1Ds.push_back(Vert);

        if(Marker != 0)
        {
            auto ret = mesh.markers_Cell1Ds.insert({Marker, {Id}});
            if(!ret.second)
                (ret.first)->second.push_back(Id);
        }
    }

    file.close();
    return true;
}

// ******************************************************************************************************************************
bool importCell2Ds(const string& fileName,
                   PolygonalMesh& mesh)
{
    ifstream file;
    file.open(fileName);
    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();
    listLines.pop_front();

    mesh.number_Cell2Ds = listLines.size();
    if (mesh.number_Cell2Ds == 0)
    {
        cerr << "ATTENTION: There is no CELL 2D!" << endl;
        return false;
    }

    mesh.id_Cell2Ds.reserve(mesh.number_Cell2Ds);
    mesh.vertices_Cell2Ds.reserve(mesh.number_Cell2Ds);
    mesh.edges_Cell2Ds.reserve(mesh.number_Cell2Ds);

    for (const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int Id;
        unsigned int Marker;
        unsigned int NumVertices;
        unsigned int NumEdges;
        vector<unsigned int> Vertices(NumVertices);
        vector<unsigned int> Edges(NumEdges);
        char del = ';';      // Definition of a delimiter

        converter >> Id >> del >> Marker >> del >> NumVertices;

        for(unsigned int i = 0; i < NumVertices; i++)   // Conversion of VERTICES
        {
            unsigned int ve;
            converter >> del >> ve;
            Vertices.push_back(ve);
        }

        converter >> NumEdges;

        for(unsigned int i = 0; i < NumEdges; i++)      // Conversion of EDGES
        {
            unsigned int ed;
            converter >> del >> ed;
            Edges.push_back(ed);
        }

        mesh.id_Cell2Ds.push_back(Id);
        mesh.vertices_Cell2Ds.push_back(Vertices);
        mesh.edges_Cell2Ds.push_back(Edges);
    }
    file.close();
    return true;
}

}
