#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <execution>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


using namespace std;

int _bredde{0};
int _dybde{0};
int _height{0};
int _smoothingIterations{0};
float dimensions[3] {0.f, 0.f, 0.f};

bool Cmp(const float a, const float b, const float epsilon = 0.0005f)
{
    return (fabs(a - b) < epsilon);
}

struct vec3
{
    vec3()
    {
        xyz[0] = 0;
        xyz[1] = 0;
        xyz[2] = 0;
    }
    vec3(float inp)
    {
        xyz[0] = inp;
        xyz[1] = inp;
        xyz[2] = inp;
    }
    vec3(float _x,float _y,float _z)
    {
        xyz[0] = _x;
        xyz[1] = _y;
        xyz[2] = _z;
    }
    float operator [] (int i) const { return xyz[i]; }
    float& operator [] (int i) { return xyz[i]; }

private:
    float xyz[3];
};

struct point
{
    float y{0.f};
    float x{0.f};
    float z{0.f};
    int counter{0};
};

void read_file(const std::string& filePath, std::vector <vec3>& points)
{
    std::ifstream file;
    file.open(filePath);

    float min[3] =    {  FLT_MAX,  FLT_MAX,  FLT_MAX };
    float max[3] =    { -FLT_MAX, -FLT_MAX, -FLT_MAX };

    if (file.is_open())
    {
        float temp[3];
        while (true)
        {
            // Siden las.txt filen blir exportert i formatet, bredde, lengde, høyde
            //    så blir innlesningstruktuen slik : x, z, y.
            file >> temp[0] >> temp[2] >> temp[1];

            // Loop som finner min og max
            for (int i = 0; i < 3; i++)
            {
                if (temp[i] > max[i])
                    max[i] = temp[i];

                else if (temp[i] < min[i])
                    min[i] = temp[i];
            }
            points.emplace_back(vec3{ temp[0],temp[1],temp[2] });

            if (file.eof())
                break;
        }
    }
    // Flytter alle punktene mot origo med å trekke fra minimum verdien for alle aksene til punktets posisjon
    for(vec3& p : points)
    {
        p[0] -= min[0];
        p[1] -= min[1];
        p[2] -= min[2];
    }

    for(int i = 0; i < 3; i++)
        dimensions[i] = max[i] - min[i];
}


void sample_points_to_grid(std::vector<vec3>& pts, const float& resolution )
{
    // Her plusser vi på 1 for å unngå floating point feil som kan føre
    //   til at et punkt er "utenfor" området vårt og krasjer applikasjonen
    const int bredde = (int)ceil(dimensions[0]/resolution)+1;
    const int hoyde =  (int)ceil(dimensions[1]/resolution)+1;
    const int dybde =  (int)ceil(dimensions[2]/resolution)+1;
    point** arr = new point*[bredde];

    for(int i = 0; i < bredde; i++)
    {
        arr[i] = new point[dybde];
        for (int j = 0; j < dybde; ++j)
        {
             arr[i][j].counter = 0;
             arr[i][j].x = i * resolution;
             arr[i][j].y = 0.f;
             arr[i][j].z = j * resolution;
        }
    }

    for(const vec3& pt : pts)
    {
        // iX og iZ er indexeringen bassert på x og z verdien til punktet pt.
        // Verdien rundes ned til nærmeste heltall
        const int iX = (int)floor(pt[0] / resolution); // Grunnen til at vi deler på oppløsning er slik at
        const int iZ = (int)floor(pt[2] / resolution); //   vi tar hennsyn til kalkulasjonene for bredde og dybde over.

        //plusser på y verdien til punktet pt inn i 2D array.
        // og legger til 1 på telleren som gjør det lettere senere å regne ut gjennomsnittet
        arr[iX][iZ].y += pt[1];
        arr[iX][iZ].counter += 1;
    }

    //Nå har vi hentet all informasjonen vi trenger fra laser.txt filen og kan derfor klarere arrayet slik at vi lett kan fylle den opp med
    // våre egne punkter.
    pts.clear();

    //Looper igjennom punktene i 2D arrayet med multithreading
    std::for_each(std::execution::par_unseq, arr, (point**)((size_t)arr + (bredde * sizeof(size_t))), [dybde](point* points) mutable
    {
        for (int z = 0; z < dybde; ++z)
        {
            auto& point = points[z];
            if(point.counter != 0)
                point.y = (point.y / (float)point.counter);
        }
    });

    // Utjevning loop.
    for(int y = 0; y < _smoothingIterations; y++)
        for(int i = 0; i < _bredde-1; i++)
            for(int j = 0; j < _dybde-1; j++)
            {
                float avg = 0;
                int counter = 0;
                if(i != 0 && j != 0)
                {
                    avg += arr[i-1][j-1].y;     counter++;
                }
                if(i > 0)
                {
                    avg += arr[i-1][j].y;       counter++;
                    avg += arr[i-1][j+1].y;     counter++;
                }
                if(j > 0)
                {
                    avg += arr[i][j-1].y;       counter++;
                    avg += arr[i+1][j-1].y;     counter++;
                }
                avg += arr[i+1][j].y;           counter++;
                avg += arr[i+1][j+1].y;         counter++;
                avg += arr[i][j+1].y;           counter++;
                avg /= (float)counter;
                arr[i][j].y = avg;
            }

    // her kan vi potensielt skrive dybde - 1 for å bli kvitt de forferdelige kantene som blir generert.
    for(int z = 0; z < dybde; z++)
    {
        for(int x = 0; x < bredde; x++)
        {
            // Fyller pts vektoren med punktene fra 2D arrayet.
            pts.emplace_back(vec3(arr[x][z].x, arr[x][z].y, arr[x][z].z));
        }
    }

    // Selvlagd sammenlignings-struct
    struct {
            bool operator()(vec3& a, vec3& b) const {

                if (a[2] < b[2]) return true;
                if (b[2] < a[2]) return false;

                if (a[0] < b[0]) return true;
                if (b[0] < a[0]) return false;

                return false;
            }
        } customLess;

    std::sort(pts.begin(), pts.end(), customLess);

    //Setter verdiene globale variabler
    _dybde = dybde;
    _bredde = bredde;

    //Finner ny høyde differens
    // Dette gjør vi ETTER utjevning steget
    float h_min {FLT_MAX};
    float h_max {-FLT_MAX};
    for(const auto& pt : pts)
    {
        if(pt[1] > h_max)
            h_max = pt[1];
        if(pt[1] < h_min)
            h_min = pt[1];
    }
    _height = h_max - h_min;

    //Sletter vi heap-allokert objekter
    for(int i = 0; i < bredde; i++)
    {
        delete[] arr[i];
    }
    delete[] arr;
}

void create_indices(std::vector<uint32_t>& indexes)
{
    for(int j = 0; j < _dybde - 1; j++)
        for(int i = 0; i < _bredde - 1; i++)
        {
            int index = j * _bredde + i;            //            3     2
            indexes.push_back(index);               //0            *---*
            indexes.push_back(index + 1);           //1            |  /|
            indexes.push_back(index + 1 + _bredde); //2            | / |
            indexes.push_back(index);               //0            |/  |
            indexes.push_back(index + 1 + _bredde); //2            *---*
            indexes.push_back(index + _bredde);     //3           0     1
        }
}

void write_to_file(std::vector<vec3>& points, std::vector<uint32_t>& indexes, const std::string& exportName)
{
    fstream file;
    file.open("../VSIM22H_LAS_TRIANGULERING/" + exportName + ".txt", ios::out);

    file << points.size() << "\n";
    for(int i = 0; i < points.size(); i++)
        file << "("<<points[i][0]<<", "<<points[i][1]<<", "<<points[i][2]<<")"<<"\n";
    //               |    x     |        |    y     |        |     z    |

    file << indexes.size()<<"\n";;
    for(int i = 0; i < indexes.size(); i++)
        file << indexes[i]<<"\n";

    file.close();
}

void write_to_hdr(std::vector<vec3>& pts, float resolution, const std::string& exportPath)
{
    // PNG/HDR exporting
    // Brukt http://chanhaeng.blogspot.com/2018/12/how-to-use-stbimagewrite.html som inspirasjon til å exportere,
    //   men jeg har gjort en rekke endringer som tilpasser vår kode/prosjekt bedre.

    const int channels {1};
    const int size = 4096 / resolution;

    float* pixels = new float[size * size * channels];
    int index {0};
    for (int j = size-1; j >= 0; --j)
    {
        for (int i = 0; i < size; ++i)
        {
            int pointIndex = j * _bredde + i;
            float r = pts[pointIndex][1]/(float)_height;
            pixels[index++] = r;
        }
    }
    std::string exportNavn = "../VSIM22H_LAS_TRIANGULERING/image_export/" + exportPath + ".hdr";
    stbi_write_hdr(exportNavn.c_str(), size, size, channels, pixels);
    delete[] pixels;
}

int main()
{
    std::vector <vec3> points;
    std::vector <uint32_t> indexes;
    std::string exportName;
    std::string importPath;

    float resolution {1.f};
    std::cout << "\t---| LAS-fil triangulering |---\n";
    std::cout << "|  Dette programmet triangulerer LAS-tekstfiler til en ny tekstfil og ett PNG/HDR heightmap\n";
    std::cout << "|  Den eksporterte tekstfilen inneholder antallet vertexer, vertex-posisjoner, og trekant-indekseringer\n|\n";
    std::cout << "|  Filadressen til LAS.txt-filen som skal trianguleres. \n|  :> ";
    std::cin >> importPath;
    std::cout << "|\n|  Navn paa eksportert fil\n|  :> ";
    std::cin >> exportName;
    std::cout << "|\n|  Mellomrom mellom punkter (i meter)\n|  :> ";
    std::cin >> resolution;
    std::cout << "|\n|  Antall utjevning-iterasjoner (0 = ingen glatting)\n|  :> ";
    std::cin >> _smoothingIterations;
    std::cout << "|\n|  Setter igang triangulering. Dette kan ta tid...\n";

    std::cout << "|  - henter punktdata\n";
    read_file(importPath, points);

    if(_smoothingIterations > 0)
        std::cout << "|  - utjevner terreng\n";
    sample_points_to_grid(points, resolution);
    std::cout << "|  - genererer indekser\n";
    create_indices(indexes);
    std::cout << "|  - skriver til tekstfil\n";
    write_to_file(points, indexes, exportName);
    std::cout << "|  - genererer heightmap (.hdr)\n|";
    write_to_hdr(points, resolution, exportName);

    std::cout << "\n|  Eksportering ferdig\n|\n";

    std::cout << "|  Terreng dimensjoner:\n";
    std::cout << "|  -  bredde (" << dimensions[0]<< " meter)\n";
    std::cout << "|  -  lengde (" << dimensions[2]<< " meter)\n";
    std::cout << "|  -  hoyde  (" << dimensions[1]<< " meter)\n\n";
    return 0;
}
