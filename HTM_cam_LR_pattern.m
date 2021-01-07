function [LeftHRight] = HTM_cam_LR_pattern(LeftHp,RightHp)
% Given two camera transformation matrices with respect to a pattern, this function is going to find out the
% transformation between the two cameras
%
% INPUT(s)
% ========
% 1. LeftHp, RightHp: Structure -- It holds rotation and translation as 'R' and 'T'
% fields, respectively. LeftHp is the tranformation between camera 1 or Left camera and the
% pattern. RightHp is the tranformation between camera 2 or Right camera and the
% pattern.
%
% OUTPUT(s)
% =========
% 1. LeftHRight: 4x4 matrix -- Transformation matrix between camera 1 or Left camera and
% camera 2 or Right Camera
%
% Example(s)
% ==========
%

%-------------------------------------------------------------------------------
%------------------------------- START -----------------------------------------

% Validate input arguments -----------------------------------------------------
p = inputParser;
p.StructExpand = false;             % Accept structure as one element

% Compulsory parameters --
addRequired(p, 'LeftHp', @(x) isstruct(x) && all(isfield(x, {'R', 'T'})));
addRequired(p, 'RightHp', @(x) isstruct(x) && all(isfield(x, {'R', 'T'})));

% Algorithm --------------------------------------------------------------------
% Convert each R|T into a homogenous matrix form\


LeftHTM = eye(4,4);                % Initialize the matrices to hold R and T
RightHTM = eye(4,4);
LeftHTM(1:3, 1:3) = LeftHp.R;       % R, T of point cloud 1
LeftHTM(1:3, 4) = LeftHp.T;
RightHTM(1:3, 1:3) = RightHp.R;       % R, T of point cloud 2
RightHTM(1:3, 4) = RightHp.T;

% Find the difference between the two transformation matrices.
% LeftHRight = LeftHTM \ RightHTM;           %Transform from A2 to A1
LeftHRight = RightHTM * inv(LeftHTM);   % H_Left2Right = H_Pat2Right * H_Left2Pat
                                        % ==> H_Left2Right = H_Pat2Right * inv(H_Pat2Left)
end