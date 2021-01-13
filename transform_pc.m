function [newPC] = transform_pc(XYZPoints,RT)
 
if(isstruct(RT))
    HTM = eye(4);
    HTM(1:3,1:3) = RT.R;
    HTM(1:3,4) = RT.T;
else
    HTM = RT;
end


XYZPoints = [XYZPoints,ones(length(XYZPoints),1)];
TransformedPoints = HTM * XYZPoints';
newPC = TransformedPoints(1:3,:)';
end
