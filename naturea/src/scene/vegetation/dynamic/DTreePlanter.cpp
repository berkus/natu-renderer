#include "DTreePlanter.h"

DTreePlanter::DTreePlanter()
{

}

DTreePlanter::~DTreePlanter(void)
{

}


void DTreePlanter::init( Terrain *_terrain,
					DTree * _tree
					
	)
{
	terrain		= _terrain;
	tree		= _tree; 
	count		= 0;
	size_factor = 1.0;
}
void DTreePlanter::createCandidates(
				float _height_min,
				float _height_max,
				float _dither,
				float _distance)
{
	// delete current candidates...
	freeCandidates.clear();
	count = 0;
	occupiedCandidates.clear();

	height_min	= _height_min;
	height_max	= _height_max;
	distance	= _distance;
	//dither		= _dither;
	dither		= 0.0;
	// tree grid sizes
	int size_grid_x = max(1, int (terrain->sz_x*size_factor / distance ));
	int size_grid_y = max(1, int (terrain->sz_y*size_factor / distance ));
	int centerOffsetX = size_grid_x/2;
	int centerOffsetY = size_grid_y/2;
	int i,j;
	float x,y,xt,yt,z,r=0.0;
	for (i=0; i<size_grid_x; i++){
		for (j=0; j<size_grid_y; j++){
			x = (i - centerOffsetX)*distance + randomf(-dither, dither);
			z = (j - centerOffsetY)*distance + randomf(-dither, dither);
			//r = randomf(-180, 180);
			xt = x + terrain->sz_x/2.0;
			yt = z + terrain->sz_y/2.0;
			y = terrain->getHeightAt(xt,yt);
			if (y<=height_max && y>=height_min){
				freeCandidates.push_back(v4(x,y,z,r));
			}
		}
	}
	

}

void DTreePlanter::setInstanceCount(int _count){
	_count = max(0,_count);
	int diff = _count - count;
	int instanceId = 0;
	if (diff>0){
		// add only available count
		diff = min(diff, int(freeCandidates.size()));


		// add
		for (int i=0; i<diff; i++){
			instanceId = randomi(0, freeCandidates.size()-1);
			v4	instance = freeCandidates[instanceId];
			freeCandidates.erase(freeCandidates.begin()+instanceId);
			occupiedCandidates.push_back(instance);
		}
		
	} else {
		// remove
		diff = min(-diff, int(occupiedCandidates.size()));

		for (int i=0; i<diff; i++){
			instanceId = randomi(0, occupiedCandidates.size());
			v4	instance = occupiedCandidates[instanceId];
			occupiedCandidates.erase(occupiedCandidates.begin()+instanceId);
			freeCandidates.push_back(instance);
		}
	}
	this->count = occupiedCandidates.size();
	g_wind_dirty = true;
	tree->initInstances(occupiedCandidates);
}