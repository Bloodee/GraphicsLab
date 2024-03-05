#include "solver.h"

#include <Eigen/Core>

using Eigen::Vector3f;

// External Force does not changed.

// Function to calculate the derivative of KineticState
KineticState derivative(const KineticState& state)
{
    return KineticState(state.velocity, state.acceleration, Eigen::Vector3f(0, 0, 0));
}

// Function to perform a single Forward Euler step
KineticState forward_euler_step([[maybe_unused]] const KineticState& previous,
                                const KineticState& current)
{
    KineticState C;

    C.acceleration        = current.acceleration;
    // �����ٶȱ仯 (v = v0 + a*dt)
    C.position= current.position + time_step * current.velocity;

    // ����λ�Ʊ仯 (x = x0 + v*dt)
    C.velocity = current.velocity + C.acceleration * time_step;

   

    return C;
}

// Function to perform a single Runge-Kutta step
KineticState runge_kutta_step([[maybe_unused]] const KineticState& previous,
                              const KineticState& current)
{
    Vector3f k1, k2, k3, k4;
    Vector3f l1, l2, l3, l4;
    Vector3f a = current.acceleration;
    // �����ĸ�����
    k1 = a * time_step;
    l1 = current.velocity * time_step;

    k2 = a * (time_step / 2.0);
    l2 = (current.velocity + k1 / 2.0) * (time_step / 2.0);

    k3 = a * (time_step / 2.0);
    l3 = (current.velocity + k2 / 2.0) * (time_step / 2.0);

    k4 = a * time_step;
    l4 = (current.velocity + k3) * time_step;

    // �����µ��ٶȺ�λ��
    Vector3f new_velocity = current.velocity + (k1 + 2.0 * k2 + 2.0 * k3 + k4) / 6.0;
    Vector3f new_position = current.position + (l1 + 2.0 * l2 + 2.0 * l3 + l4) / 6.0;

    return KineticState(new_position, new_velocity, a);

}

// Function to perform a single Backward Euler step
KineticState backward_euler_step([[maybe_unused]] const KineticState& previous,
                                 const KineticState& current)
{

    Vector3f new_velocity = current.velocity; // ��ʼ�²�
    double tolerance    = 1e-6;             // �ݲ�
    double error        = 1.0;              // ���
    int max_iter        = 100;              // ����������
    int iter            = 0;                // ��ǰ��������
    Vector3f a            = current.acceleration;

    // ����ֱ�������򳬹�������
    while (error > tolerance && iter < max_iter) {
        // ���㺯��ֵ�͵���ֵ
        Vector3f f  = new_velocity - current.velocity - a * time_step;
        Vector3f df =
            Vector3f{
                1.0,
            1.0,1.0} -
            a * time_step;

        // �����µ��ٶȺ����
        Vector3f old_velocity = new_velocity;
        new_velocity          = old_velocity - Vector3f{f[0] / df[0] ,f[1] / df[1], f[2] / df[2]};
        error               = (new_velocity - old_velocity).norm();

        // ���ӵ�������
        iter++;
    }

    // �����µ�λ��
    Vector3f new_position = current.position + new_velocity * time_step;

    // �����µ�״̬
    return KineticState(new_position, new_velocity, a);

}

// Function to perform a single Symplectic Euler step
KineticState symplectic_euler_step(const KineticState& previous, const KineticState& current)
{
    (void)previous;
    // �����µ��ٶȺ�λ��
    Vector3f a          = current.acceleration;
    Vector3f new_velocity = current.velocity + a * time_step;
    Vector3f new_position = current.position + new_velocity * time_step;

    // �����µ�״̬
    return KineticState(new_position, new_velocity, a);
}
