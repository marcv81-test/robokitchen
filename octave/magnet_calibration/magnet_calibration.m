% magnetometer calibration
function [hard, soft, error] = magnet_calibration(data)

  % ellipsoid fit
  [center, radii, axes] = ellipsoid_fit(data);

  % hard-iron correction vector
  hard = center;

  % soft-iron correction matrix
  scale = [1/radii(1) 0 0; 0 1/radii(2) 0; 0 0 1/radii(3)];
  soft = axes*(scale*transpose(axes));

  % radius error per data sample
  data_size = size(data, 1);
  error = zeros(data_size, 1);
  for i = 1:data_size,
    sample = transpose(data(i, :));
    error(i) = 1.0 - norm(soft*(sample-hard));
  end

end
