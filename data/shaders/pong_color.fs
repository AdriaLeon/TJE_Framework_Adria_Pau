
varying vec3 v_world_position;
varying vec3 v_normal;

uniform vec3 u_Kd;
uniform vec3 u_Ka;
uniform vec3 u_Ks;
uniform vec3 ambient_light;
uniform vec3 u_light_color;
uniform vec3 u_light_direction;
uniform vec3 u_camera_pos;
uniform float shininess;

void main()
{
    vec4 Color = vec4(u_Kd, 1.0);
    
    vec3 N = normalize(v_normal);
    vec3 V = normalize(u_camera_pos - v_world_position);
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

    // Output final color
    gl_FragColor = vec4(Color.xyz * light, 1.0);
}
