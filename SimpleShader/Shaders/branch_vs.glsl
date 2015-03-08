#
uniform sampler2DRect	data_tex;
uniform vec2			A;
uniform float			time;
attribute vec3			binormal;
/*
vec4 RED = vec4(1.0, 0.0, 0.0, 1.0);
vec4 GREEN = vec4(0.0, 1.0, 0.0, 1.0);
vec4 BLUE = vec4(0.0, 0.0, 1.0, 1.0);
vec4 YELLOW = vec4(1.0, 1.0, 0.0, 1.0);

*/
void readBranchData(in float index, out vec3 o, out vec3 r, out vec3 s, out vec3 t, out vec2 motionVec, out float bx, out float pID, out float level, out float c2, out float c4, out float L){
	o = texture2DRect( data_tex, vec2(0.5,index) ).xyz;
	r = texture2DRect( data_tex, vec2(1.5,index) ).xyz;
	s = texture2DRect( data_tex, vec2(2.5,index) ).xyz;
	t = texture2DRect( data_tex, vec2(3.5,index) ).xyz;
	vec3 mixed1 = texture2DRect( data_tex, vec2(4.5,index) ).xyz;
	vec3 mixed2 = texture2DRect( data_tex, vec2(5.5,index) ).xyz;
	motionVec = texture2DRect( data_tex, vec2(6.5,index) ).xy;
	
	bx	= mixed1.x;
	pID	= mixed1.y+0.5;
	level= mixed1.z;
	c2	= mixed2.x;
	c4	= mixed2.y;
	L	= mixed2.z;
}

float readParentIndex(in float index){
	return texture2DRect( data_tex, vec2(4.5,index) ).y + 0.5;
}

void bendCoordSystem( in float bx, in float bPid, in float bC2, in float bC4, in float bL, in vec2 bA, in vec3 bO, in vec3 bR, in vec3 bS, in vec3 bT, in float vx, inout vec3 vO, inout vec3 R, inout vec3 S, inout vec3 T ){
	//float rr,rs,rt,sr,ss,st,tr,ts,tt;
	float Ar, As;
	Ar = bA.x*sin(time);
	As = 0.0;//bA.y * sin(time);

	// tranform branch coord system
	float xr,xs,ux, uxs, uxr, uxsd, uxrd, uxd, sxr, sxs, dxr, dxs;	
	ux		= bC2*vx*vx + bC4*vx*vx*vx*vx; // u(x)
	uxd		= 2.0*bC2*vx + 3.0*bC4*vx*vx*vx; // du(x)/dx
	uxrd	= Ar*uxd/bL;
	uxsd	= As*uxd/bL;
	uxr		= Ar*ux;
	uxs		= As*ux;
	sxr		= sqrt(1.0+uxrd*uxrd);
	sxs		= sqrt(1.0+uxsd*uxsd);
	dxr		= uxrd!=0.0?uxr/uxrd*(sxr-1.0):0.0;
	dxs		= uxsd!=0.0?uxs/uxsd*(sxs-1.0):0.0;
	
	// tangent & normal recalculation
	xr		= vx - dxr/sxr;
	xs		= vx - dxs/sxs;
	uxrd	= Ar*(2.0*bC2*xr + 3.0*bC4*xr*xr*xr)/bL;
	uxsd	= As*(2.0*bC2*xs + 3.0*bC4*xs*xs*xs)/bL;
	
	vec3 t2	= normalize(bT + (bR*uxrd + bS*uxsd));
	vec3 r2	= normalize(bR - bT*uxrd);
	vec3 s2	= normalize(bS - bT*uxsd);

	// calculate output:

	//CS origin
	vO = bO + r2*vO.x + s2*vO.y + bT*vO.z - (bT * dxr - bR * uxr)/sxr - (bT * dxs - bS * uxs)/sxs;
	
	//CS base vectors
	//vR = normalize(r2*rr + s2*rs + t2*rt);
	//vS = normalize(r2*sr + s2*ss + t2*st);
	//vT = normalize(r2*tr + s2*ts + t2*tt);	
	//R = normalize(r2*R.x + s2*R.y + t2*R.z);
	//S = normalize(r2*S.x + s2*S.y + t2*S.z);
	//T = normalize(r2*T.x + s2*T.y + t2*T.z);	
	R = r2*R.x + s2*R.y + t2*R.z;
	S = r2*S.x + s2*S.y + t2*S.z;
	T = r2*T.x + s2*T.y + t2*T.z;
}			



void main()
{
	vec4 vertex = gl_Vertex;
	//vec4 values = texture2DRect( data_tex, gl_MultiTexCoord0.xy );
	
	float vx = gl_MultiTexCoord2.x;
	// actual branch data
	vec3 bo, br, bs, bt;
	float bx , bpid, blevel;
	vec2 bmv;
	float bc2, bc4, bL;
	// parent branch data
	vec3 po, pr, ps, pt;
	float px , ppid, plevel;
	vec2 pmv;
	float pc2, pc4, pL;

	// create simple vertex coord system to bend
	vec3 vo = vertex.xyz;
//TO DO: subst. with real normal and tangent:
	vec3 vr = vec3(1.0, 0.0, 0.0);
	vec3 vs = vec3(0.0, 1.0, 0.0);
	vec3 vt = vec3(0.0, 0.0, 1.0);
	
		
	// extract hierarchy data 
	const int HIER_DEPTH = 5;
	float indices[HIER_DEPTH];
	indices[0] = gl_MultiTexCoord2.y+0.5;
	int i,j;
	for (i=1; i<HIER_DEPTH; i++){
		indices[i] = readParentIndex(indices[i-1]);
		
		if (indices[i]<0.0){
			// no parent
			i = i-1;
			break;
		}
		
	}
	// read trunk data as 1st parent
	readBranchData(indices[i], po, pr, ps, pt, pmv, px, ppid, plevel, pc2, pc4, pL);
	// hierarchical transform	
	for (j=i-1; j>=0; j--){
		// read actual coord system (CS)
		readBranchData(indices[j], bo, br, bs, bt, bmv, bx, bpid, blevel, bc2,bc4, bL);

		// bend actual CS in parent CS
		bendCoordSystem( bx, ppid, pc2, pc4, pL, pmv, po, pr, ps, pt, bx, bo, br, bs, bt);
		// propagate to next transform step
		po	= bo;
		pr	= br;
		ps	= bs;
		pt	= bt;
		pc2	= bc2;
		pc4	= bc4;
		pL	= bL;
		pmv	= bmv;
		// has parent
	} 
	// bend given vertex CS in final CS
	bendCoordSystem( bx, ppid, pc2, pc4, pL, pmv, po, pr, ps, pt, vx, vo, vr, vs, vt);

	vertex = vec4(vo,1.0);
	// construct output
	gl_FrontColor = gl_Color;//vec4(time*0.01,0.0,0.0,1.0);
	gl_Position = gl_ModelViewProjectionMatrix * vertex;
}
