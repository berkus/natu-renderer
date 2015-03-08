#include "Planter.h"

Planter::Planter()
{

}

Planter::~Planter(void)
{

}


void Planter::init( Terrain*	_terrain,
					Vegetation* _prototype, 
					Vegetation*	_growth,
					float		_height_min,
					float		_height_max,
					float		_minDist,
					int			_res_x,
					int			_res_y)
{
	terrain		= _terrain;
	prototype	= _prototype; 
	growth		= _growth;
	height_min	= _height_min;
	height_max	= _height_max;
	minDist		= _minDist;
	res_x		= _res_x;
	res_y		= _res_y;	
	count		= 0;

	recompute();
}

void Planter::setNewMin(float _min) {
	height_min = _min;
	recompute();
	int c2 = count;
	count = 0;
	plantVegetationCount(c2);
}

void Planter::setNewMax(float _max) {
	height_max = _max;
	recompute();
	int c2 = count;
	count = 0;
	plantVegetationCount(c2);
}

void Planter::recompute() {
	candidates.clear();
	realPositions.clear();
	float tsx	= terrain->sz_x/2.0;
	float tsy	= terrain->sz_y/2.0;
	// get candidates array
	float step_x = terrain->sz_x/float(res_x);
	float step_y = terrain->sz_y/float(res_y);
	int xi, yi;
	for (xi=0; xi<res_x; xi++)
	{
		for (yi=0; yi<res_y; yi++)
		{
			float x = randomf(-step_x*0.5f, step_x*0.5f);
			float y = randomf(-step_y*0.5f, step_y*0.5f);
			v3  pos(xi*step_x+x, 0.f , yi*step_y+y);
			float height = terrain->getHeightAt(xi*step_x+x, yi*step_y+y);
			if (height>height_min && height<height_max){
				pos.y=height;
				pos.x -= tsx;
				pos.z -= tsy;
				candidates.push_back(pos); 
			}
		}
	}
}

int Planter::plantVegetationCount(int _count)
{
	// if count > max ... no change
	// if count <0		... 
	int diff = _count-count;
	printf("diff = %i\n", diff);
	int i;
	if (diff>0){
		// add
		for (i=0; i<diff; i++){
			add();
		}
		printf("count = %i\n", count);
	} else if (diff<0){
		// erase
		for(i=diff; i<0; i++){
			erase();
		}
	}
	
	// prepare for VBO
	growth->vertices.clear();
	count = 0;
	for (int i=0; i<realPositions.size(); i++){
		
		v3 candidate = realPositions[i];
		m4 transform;
		transform.scale(v3(randomf(0.6f, 1.0f)));
		transform.rotate(v3(0.f, 1.f, 0.f), randomf(0.f, PI/4.f));
		transform.translate(candidate);
		prototype->fixTexType();
		// transform all vertices of vegetation
		for (int i=0; i<prototype->VBOdataCount; i++)
		{
			VertexInfo v(prototype->vertices[i]);
			//printf("pos: %f, %f, %f, %f\n", v.position.x, v.position.y, v.position.z, v.position.w);
			v.position = transform*v.position;
			//printf("popos: %f, %f, %f, %f\n", v.position.x, v.position.y, v.position.z, v.position.w);
			v.texCoord = prototype->transformTexCoords(v.texCoord);	
			v.normal   = (transform*v.normal);
			v.normal.normalize();
			growth->vertices.push_back(v);
		}
		count++;
		//int k = printf("VEG count: %i", count);
		//BACKSPACE(k);
	}
	bakeVBO();
	return count;
}

void Planter::add()
{
	if (candidates.size()>0){
		int r = randomi(0, candidates.size()-1);
		if (r>=candidates.size()){
			printf("chyba (r=%i, size=%i)\n", r, candidates.size());
			system("PAUSE");
		}
		v3 candidate = candidates[r];
		realPositions.push_back(candidate);
		candidates.erase(candidates.begin()+r);
		// erase candidates closer than
		for (int i = 0; i<candidates.size(); i++){
			v3 can = candidates[i];
			v3 distV = candidate - can;
			float distance = distV.length();
			if (distance<minDist){
				// invalid
				candidates.erase(candidates.begin()+i);
				i--;
			}
		}

		// save position
		m4 transform;
		//transform.scale(v3(randomf(0.3f, 1.1f)));
		//transform.rotate(v3(0.f, 1.f, 0.f), randomf(0.f, PI/4.f));
		transform.translate(candidate);
		prototype->fixTexType();
		// transform all vertices of vegetation prototype
		for (int i=0; i<prototype->VBOdataCount; i++)
		{
			VertexInfo v(prototype->vertices[i]);
			//printf("pos: %f, %f, %f, %f\n", v.position.x, v.position.y, v.position.z, v.position.w);
			v.position = transform*v.position;
			//printf("popos: %f, %f, %f, %f\n", v.position.x, v.position.y, v.position.z, v.position.w);
			v.texCoord = prototype->transformTexCoords(v.texCoord);	
			v.normal   = (transform*v.normal);
			v.normal.normalize();
			growth->vertices.push_back(v);
		}
		count++;
	}
}
void Planter::bakeVBO(){
	growth->bakeToVBO();
}

void Planter::erase()
{
	if (realPositions.size()>0){
		candidates.push_back(realPositions[realPositions.size()-1]);
		realPositions.erase(realPositions.end()-1);
	}
}

int Planter::plantVegetation(Vegetation* prototype, Vegetation *growth)
{
	// fill all available place with prototype copy...
	int count = 0;
	float tsx = terrain->sz_x/2.0;
	float tsy = terrain->sz_y/2.0;
	v3 distVector;

	// get candidates array
	float step_x = terrain->sz_x/float(res_x);
	float step_y = terrain->sz_y/float(res_y);
	int xi, yi;
	for (xi=0; xi<res_x; xi++)
	{
		for (yi=0; yi<res_y; yi++)
		{
			float x = randomf(-step_x*0.5f, step_x*0.5f);
			float y = randomf(-step_y*0.5f, step_y*0.5f);
			v3  pos(xi*step_x+x, 0.f , yi*step_y+y);
			float height = terrain->getHeightAt(xi*step_x+x, yi*step_y+y);
			if (height>height_min && height<height_max){
				pos.y=height;
				pos.x -= tsx;
				pos.z -= tsy;
				candidates.push_back(pos); 
			}
		}
	}
	// get positions from candidates
	while (!candidates.empty()){
		// select candidate
		v3 candidate = candidates[candidates.size()-1];
		
		// delete all neighbour candidates closer than minDist
		for (int i=candidates.size()-1; i>=0; i-- ){
			v3 distVector = candidate - candidates[i];
			float dist = distVector.length();
			if (dist<minDist){
				// closer -> delete
				candidates.erase(candidates.begin()+i);
			}
		}
	} // end while
	return count;
}
