/**
 * @file maptiles.cpp
 * Code for the maptiles function.
 */

#include <iostream>
#include <map>
#include "maptiles.h"
//#include "cs225/RGB_HSL.h"

using namespace std;


Point<3> convertToXYZ(LUVAPixel pixel) {
    return Point<3>( pixel.l, pixel.u, pixel.v );
}

MosaicCanvas* mapTiles(SourceImage const& theSource,
                       vector<TileImage>& theTiles)
{
    /**
     * @todo Implement this function!
     */
    //ROWS AND COLUMNS CAN BE -1
    int rows = theSource.getRows();
    int columns = theSource.getColumns();
    MosaicCanvas* mosaicCanvas = new MosaicCanvas(rows, columns);

    //Transforming tiles to Points and storing them in vector.
    vector<Point<3>> tilesAsPoints;
    //Allows for easy transformation from point to the tile image associated with it.
    map<Point<3>, TileImage*> pointToImage;

    for (size_t i = 0; i < theTiles.size(); ++i) {
        TileImage& tile = theTiles[i];

        LUVAPixel pixel = tile.getAverageColor();
        double point_arr[3] = {pixel.l, pixel.u, pixel.v};
        Point<3> pixel_point(point_arr);
        //Adds to vector of points and map of points to tiles.
        tilesAsPoints.push_back(pixel_point);
        pointToImage.insert(pair<Point<3>, TileImage*>(tilesAsPoints[i], &tile));
    }
    //Making KDTree out of points vector
    KDTree<3> tree(tilesAsPoints);
    //Iterates through every region in theSource and gets the tile for it.
    for (int row_index = 0; row_index < rows; ++row_index) {
        for (int column_index = 0; column_index < columns; ++column_index) {
            //Finds average region Point/Target.
            LUVAPixel pixel = theSource.getRegionColor(row_index, column_index);
            double point_arr[3] = {pixel.l, pixel.u, pixel.v};
            Point<3> pixel_point(point_arr);

            Point<3> nearest = tree.findNearestNeighbor(pixel_point);
            mosaicCanvas->setTile(row_index, column_index, pointToImage[nearest]);
        }
    }
    return mosaicCanvas;
}

