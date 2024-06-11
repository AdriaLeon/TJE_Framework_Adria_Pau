
varying vec3 v_world_position;
varying vec3 v_normal;

uniform vec3 u_Kd;
uniform vec3 u_Ka;
uniform vec3 u_Ks;
uniform vec3 ambient_light;
uniform vec3 u_light_color;
uniform vec3 u_light_direction;
uniform vec3 u_camera_position;
uniform float shininess;

//Fog
uniform float fogMaxDist;
uniform float fogMinDist;
uniform vec3 fog_color;


void main()
{
    vec4 Color = vec4(u_Kd, 1.0);
    
    vec3 N = normalize(v_normal);
    vec3 V = normalize(u_camera_position - v_world_position);
    vec3 L = normalize(u_light_direction);
    vec3 R = reflect(-L, N);

    // Ambient
    vec3 ambient = u_Ka * ambient_light;

    // Diffuse
    float NdotL = clamp(dot(N, L), 0.0, 1.0);
    vec3 diffuse = u_Kd * u_light_color * NdotL;

    // Specular
    float RdotV = pow(max(0.0, dot(R, V)), shininess);
    vec3 specular = u_Ks * u_light_color * RdotV;

    // Final light calculation
    vec3 light = ambient + diffuse + specular;

    //Add light
    Color.xyz = Color.xyz * light;

    // Pass to the shader the fog color and the min/max dist
    float dist = length(u_camera_position - v_world_position);
    float fog_factor = clamp( 1.0 - abs(fogMaxDist - dist) / (fogMaxDist - fogMinDist), 0.0, 1.0);
    vec3 final_color = mix(Color.xyz, fog_color, fog_factor);

    // Output final color
    gl_FragColor = vec4(final_color.xyz, 1.0);
}
