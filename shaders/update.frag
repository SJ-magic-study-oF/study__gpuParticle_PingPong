#version 120

#extension GL_ARB_texture_rectangle : enable
#extension GL_ARB_draw_buffers : enable

#pragma include "util.frag"
#pragma include "noise4D.frag"

uniform sampler2DRect u_posAndAgeTex;
uniform sampler2DRect u_velAndMaxAgeTex;

uniform float u_time;
uniform float u_timestep;
uniform float u_scale;
uniform vec3 u_emitterPos;
uniform vec3 u_prevEmitterPos;

void main(void){
	vec2 st = gl_TexCoord[0].st;
	
	// 前の位置情報とパーティクル初期化からの経過時間を取得
	vec4 posAndAge = texture2DRect(u_posAndAgeTex,st);
	
	// 前の速度と生存期間を取得
	vec4 velAndMaxAge = texture2DRect(u_velAndMaxAgeTex,st);
	
	vec3 pos = posAndAge.xyz; // 前の位置
	vec3 vel = velAndMaxAge.xyz; // 前の速度
	
	float age = posAndAge.w; // 経過時間
	float maxAge = velAndMaxAge.w; // 生存期間

	age++;
	
	// パーティクルが生存期間を過ぎたら初期化
	if(maxAge <= age){
		/********************
		このblock内のrandom()で使われるseedsは、何でも良い.
		posを使用しているが、特にposと意味的に結びついている訳ではない.
		それぞれのrandom()でseedsがダブらないようにしているだけ.
		********************/
		age = 0;
		maxAge = 50.0 + 250.0 * random(pos.xx);
		
		/********************
		同じ or 近くの time frameに寿命が来た場合は、startPosも近い位置に設定.
		u_emitterPosは、一辺300pixelの立方体内でperlin noiseを用いて漂っている.
		********************/
		vec3 startPos = u_prevEmitterPos + (u_emitterPos - u_prevEmitterPos) * random(pos.yz);
		
		/********************
		startPosを中心に、半径 5.0 pix の球内のどこか.
		********************/
		float theta = 2.0 * PI * random(pos.yy);
		float phi = PI * random(pos.zz);
		float r = 5.0 * random(pos.xy);
		pos = startPos + vec3(r * sin(theta) * cos(phi), r * sin(theta) * sin(phi), r * cos(theta));
		
		/********************
		大きさ 1 で、原点から離れる方向.
		********************/
		vel.xyz = vec3(normalize(startPos));
	}
	
	/********************
	Curl Noiseで速度を更新
	
	posと経過時間からvelを算出している.
	全てのposについて、初期位置は、([-1, 1], [-1, 1], [-1, 1]).
	velの初期値は、(0, 0).
	
	なので、最初の消滅までは、全てのparticleがほぼ同じ動きになる
		velAndMaxAge[i*4 + 3] = ofRandom(50,150);
		などとして、少なくとも最初の50frame、消滅がないようにしてみると、よくわかる.
	そこで実際には、
		velAndMaxAge[i*4 + 3] = ofRandom(1,150);
	と、一瞬で消滅するparticleも含めることで、上手くparticleを散らしている.
	********************/
	vel.x += snoise(vec4(pos.x * u_scale, pos.y * u_scale, pos.z * u_scale, 0.1352 + u_time * u_timestep));
	vel.y += snoise(vec4(pos.x * u_scale, pos.y * u_scale, pos.z * u_scale, 1.2814 + u_time * u_timestep));
	vel.z += snoise(vec4(pos.x * u_scale, pos.y * u_scale, pos.z * u_scale, 2.5564 + u_time * u_timestep));
	
	pos += vel;
	
	gl_FragData[0].rgba = vec4(pos, age); // 位置と経過時間を出力
	gl_FragData[1].rgba = vec4(vel, maxAge); //速度と生存期間を出力
}
