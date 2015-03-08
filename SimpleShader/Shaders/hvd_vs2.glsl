#
uniform sampler2DRect	data_tex;
uniform vec2			dataTexture_steps;
uniform vec2			A;

void readBranchData(in float index, out vec3 o, out vec3 r, out vec3 s, out vec3 t, out vec2 motionVec, out float bx, out float pID, out float level, out float c2, out float c4, out float L){
	o = texture2DRect( data_tex, vec2(gl_MultiTexCoord0.x,0.5) ).xyz;
	r = texture2DRect( data_tex, vec2(gl_MultiTexCoord0.x,1.5) ).xyz;
	s = texture2DRect( data_tex, vec2(gl_MultiTexCoord0.x,2.5) ).xyz;
	t = texture2DRect( data_tex, vec2(gl_MultiTexCoord0.x,3.5) ).xyz;
	vec3 mixed1 = texture2DRect( data_tex, vec2(gl_MultiTexCoord0.x,4.5) ).xyz;
	vec3 mixed2 = texture2DRect( data_tex, vec2(gl_MultiTexCoord0.x,5.5) ).xyz;
	motionVec = texture2DRect( data_tex, vec2(gl_MultiTexCoord0.x,6.5) ).xy;
	
	bx	= mixed1.x;
	pID	= mixed1.y+0.5;
	level= mixed1.z;
	c2	= mixed2.x;
	c4	= mixed2.y;
	L	= mixed2.z;
}

void bendCoordSystem( in float bx, in float bPid, in float bC2, in float bC4, in float bL, in vec2 bA, in vec3 bO, in vec3 bR, in vec3 bS, in vec3 bT, in float vx, inout vec3 vO, inout vec3 vR, inout vec3 vS, inout vec3 vT ){
	float rr,rs,rt,sr,ss,st,tr,ts,tt;
	rr = dot(vR, bR);
	rs = dot(vR, bS);
	rt = dot(vR, bT);

	sr = dot(vS, bR);
	ss = dot(vS, bS);
	st = dot(vS, bT);

	tr = dot(vT, bR);
	ts = dot(vT, bS);
	tt = dot(vT, bT);	
	
	// if not in trunk, bend all parent branches coord. systems
	if (bPid>=0.0){
		// read data of parent branch
		vec3 o, r, s, t;
		float x, pid, pLevel, c2, c4, L;
		vec2 motionTexVector;
		readBranchData(gl_MultiTexCoord0.y+0.5, o, r, s, t, motionTexVector, x, pid, pLevel, c2,c4, L);
		// bend branch coord system in parent branch
		bendCoordSystem( x, pid, c2, c4, L, motionTexVector, o, r, s, t, bx, bO, bR, bS, bT );
	} 
	
	// tranform branch coord system
	float xr,xs,ux, uxs, uxr, uxsd, uxrd, uxd, sxr, sxs, dxr, dxs;	
	ux		= bC2*vx*vx + bC4*vx*vx*vx*vx; // u(x)
	uxd		= 2.0*bC2*vx + 3.0*bC4*vx*vx*vx; // du(x)/dx
	uxrd	= bA.x*uxd/bL;
	uxsd	= bA.y*uxd/bL;
	uxr		= bA.x*ux;
	uxs		= bA.y*ux;
	sxr		= sqrt(1.0+uxrd*uxrd);
	sxs		= sqrt(1.0+uxsd*uxsd);
	dxr		= uxrd!=0.0?uxr/uxrd*(sxr-1.0):0.0;
	dxs		= uxsd!=0.0?uxs/uxsd*(sxs-1.0):0.0;
	
	// tangent & normal recalculation
	xr		= vx - dxr/sxr;
	xs		= vx - dxs/sxs;
	uxrd	= bA.x*(2.0*bC2*xr + 3.0*bC4*xr*xr*xr)/bL;
	uxsd	= bA.y*(2.0*bC2*xs + 3.0*bC4*xs*xs*xs)/bL;
	
	vec3 t2	= normalize(bT + (bR*uxrd + bS*uxsd));
	vec3 r2	= normalize(bR - bT*uxrd);
	vec3 s2	= normalize(bS - bT*uxsd);

	// calculate output
	vO = bO + bR*vO.x + bS*vO.y + bT*vO.z - (bT * dxr - bR * uxr)/sxr - (bT * dxs - bS * uxs)/sxs;
	
	vR = normalize(r2*rr + s2*rs + t2*rt);
	vS = normalize(r2*sr + s2*ss + t2*st);
	vT = normalize(r2*tr + s2*ts + t2*tt);	
}	
void main()
{
	vec4 vertex = gl_Vertex;
	float vx = gl_MultiTexCoord0.x;
	vec3 bo, br, bs, bt;
	float bx , pid, level, c2, c4, L;
	vec2 motionTexVector;
	readBranchData(gl_MultiTexCoord0.y+0.5, bo, br, bs, bt, motionTexVector, bx, pid, level, c2,c4, L);
	// create simple vertex coord system to bend
	vec3 vo = vertex.xyz;
	vec3 vr = vec3(1.0, 0.0, 0.0);
	vec3 vs = vec3(0.0, 1.0, 0.0);
	vec3 vt = vec3(0.0, 0.0, 1.0);
	bendCoordSystem( bx, pid, c2, c4, L, motionTexVector, bo, br, bs, bt, vx, vo, vr, vs, vt);
	//vertex = vec4(vo,1.0);
	gl_FrontColor = gl_Color; 
	gl_Position = gl_ModelViewProjectionMatrix * vertex;
}
