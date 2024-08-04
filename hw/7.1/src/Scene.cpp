//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"


void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}

int count = 0;

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    // TODO Implement Path Tracing Algorithm here
    Vector3f color(0);

    // check hit with scene
    Intersection inter_ray = intersect(ray);

    if (!inter_ray.happened) {
        return color;
    }

    // hit light source
    if (inter_ray.m->hasEmission()) {
        return inter_ray.m->getEmission();
    }

    const float epsilon = 0.0005f;

    // ray hit an object, perform path tracing on hit (shading) point
    // member for shading point
    Vector3f p = inter_ray.coords;  // shading point
    Vector3f wo = normalize(-ray.direction);  // observation dir.
    Material* m = inter_ray.m;  // material at shading point
    Vector3f N = normalize(inter_ray.normal); // normal of shading point  
    
    switch (m->getType()) {
        case DIFFUSE: {
            // Direct Illumination
            Vector3f L_dir(0);
            if (m->getType() == DIFFUSE) {
                
                // sample light
                Intersection inter_light;
                float pdf_light;
                sampleLight(inter_light, pdf_light);
                Vector3f x = inter_light.coords;  // sampled position
                Vector3f ws_unorm = x - p;  // shading point -> source
                Vector3f ws = normalize(ws_unorm);  // shading point -> source
                Vector3f NN = normalize(inter_light.normal); // normal of sampled position

                // check block between shading point & sampled light position
                Intersection inter_dir = intersect(Ray(p, ws));
                // no block, add direct illumination
                // if (inter_dir.obj == inter_light.obj)
                if (inter_dir.distance - ws_unorm.norm() > -epsilon)
                {
                    // apply rendering equation (dir. part)
                    L_dir = inter_light.emit 
                        * m->eval(ws, wo, N) 
                        * std::max(.0f, dotProduct(ws, N))
                        * std::max(.0f, dotProduct(-ws, NN))
                        / dotProduct(ws_unorm, ws_unorm) 
                        / pdf_light;
                }
            }

            // Indirect Illumination
            Vector3f L_indir(0);
            float p_RR = get_random_float();
            if (p_RR < RussianRoulette)
            {
                Vector3f wi = m->sample(ray.direction, N).normalized();  // sample a direction for indierect illumination
                Ray next_ray(p, wi);
                Intersection next_inter = intersect(next_ray);
                if (next_inter.happened && !next_inter.m->hasEmission())
                {
                    // apply rendering equation (indir. part)
                    float pdf = m->pdf(ray.direction, wi, N);
                    if (pdf > epsilon)
                    {
                        L_indir = castRay(next_ray, depth + 1)  // cast ray recursively
                                * m->eval(ray.direction, wi, N) 
                                * dotProduct(wi, N)
                                / pdf 
                                / RussianRoulette; // remove an extra division by 
                                                   // RussionRoulette according to 
                                                   // https://github.com/MARMOTatZJU/GAMES101-HW/issues/2
                    }
                }
            }

            color = L_dir + L_indir;
            break;
        }

        // https://blog.csdn.net/ycrsw/article/details/124408789
        case MIRROR: {
            Vector3f L_indir(0);
            float p_RR = get_random_float();
            if (p_RR < RussianRoulette)
            {
                Vector3f wi = m->sample(ray.direction, N).normalized();  // sample a direction for indierect illumination
                Ray next_ray(p, wi);
                Intersection next_inter = intersect(next_ray);
                if (next_inter.happened)
                {
                    // apply rendering equation (indir. part)
                    float pdf = m->pdf(ray.direction, wi, N);
                    if (pdf > epsilon)
                    {
                        L_indir = castRay(next_ray, depth + 1)  // cast ray recursively
                                * m->eval(ray.direction, wi, N) 
                                * dotProduct(wi, N)
                                / pdf 
                                / RussianRoulette; // remove an extra division by 
                                                   // RussionRoulette according to 
                                                   // https://github.com/MARMOTatZJU/GAMES101-HW/issues/2
                    }
                }
            }

            color = L_indir;
            break;
        }

        default:
            break;
    }

    return color;
}