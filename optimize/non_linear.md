# 非线性拟合求解最小值

针对如下的非线性函数最小值优化问题：
$$
min_x \frac{1}{2}\sum_k\rho(||f(x)||^2)
$$

一般核函数$\rho$ 直接为常数1，表示不对结果进行变换，优化问题变为：
$$
min_x \frac{1}{2}\sum_k||f(x)||^2
$$


泰勒展开可得：
$$f(x+\Delta x) \approx f(x) + f'(x)\Delta x$$
$$f=f(x)$$
$$J=f'(x)$$

$$F=\frac{1}{2}||f(x)||^2\approx(f+J\Delta x)^2=f^2+2fJ\Delta x + \Delta x^TJ^TJ\Delta x$$

令其一阶导数为0，得到normal equation

$$(J^TJ)\Delta x=-J^Tf$$


转换为LM算法形式：
$$(J^TJ + \mu I)\Delta x=-J^Tf$$

$$Delta x=-(J^TJ+\mu I)^{-1}J^Tf$$

梯度下降，其中$\alpha$表示迭代步长：
$$Delta x=-\alpha J^Tf$$


高斯-牛顿方法：
$$Delta x=-(J^TJ)^{-1}J^Tf$$


动手实现三种迭代算法