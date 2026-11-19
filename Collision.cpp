#include "Collision.h"

bool coll::circleRectIntersect(sf::CircleShape circle, sf::Rect<float> rectangle)
{
    float circleDistX = std::fabs(circle.getPosition().x - rectangle.position.x);
    float circleDistY = std::fabs(circle.getPosition().y - rectangle.position.y);
    
    if (circleDistX > (rectangle.size.x / 2.f + circle.getRadius())) { return false; }
    if (circleDistY > (rectangle.size.y / 2.f + circle.getRadius())) { return false; }
    if (circleDistX <= (rectangle.size.x / 2.f)) { return true; }
    if (circleDistY <= (rectangle.size.y / 2.f)) { return true; }
    
    float cornerDist = std::powf(circleDistX - rectangle.size.x / 2.f, 2.f) + std::powf(circleDistY - rectangle.size.y / 2.f, 2.f);
    return (cornerDist <= std::powf(circle.getRadius(), 2.f));
}
