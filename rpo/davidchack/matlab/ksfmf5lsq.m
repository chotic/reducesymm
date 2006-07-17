function [g, jac] = ksfmf5lsq(t, a, d, h)%, C, alpha);
%  function g = ksfmf5lsq(t, a, d, h)  -  Associated flow for the full KSFM
%  5 - for quasiperiodic orbits and use with lsqnonlin

global TT A F FT NFEVAL PHASE DD
  
  na = length(a)-2;
  if nargout == 1, NFEVAL = NFEVAL + 1;
    [TT, A] = ksfmedt(d, a(end-1), a(1:end-2), h, 2);
%    FT = ksfm(t, A(:,end), d);
  else,  NFEVAL = NFEVAL + na + 3;
    [TT, A, DA] = ksfmjedt(d, a(end-1), a(1:end-2), h, 2);
%    [FT, DF] = ksfm(t, A(:,end), d);
  end
  FT = ksfm(t, A(:,end), d);  PHASE = a(end);  DD = d;
  
  k = (-2i*pi/d).*(1:na/2)';  ek = exp(k.*PHASE);
  cg = ek.*(A(1:2:end,end)+1i*A(2:2:end,end));
  
  g = a(1:end-2);  g(1:2:end) = real(cg) - g(1:2:end);
  g(2:2:end) = imag(cg) - g(2:2:end);

  if nargout > 1,
    cf = ek.*(FT(1:2:end-1)+1i*FT(2:2:end));  dtg = zeros(na,1);
    dtg(1:2:end) = real(cf);  dtg(2:2:end) = imag(cf);
  
    cg = k.*cg;  ddg = zeros(na,1);
    ddg(1:2:end) = real(cg);  ddg(2:2:end) = imag(cg);
  
    dug = eye(na);
    cdg = repmat(ek,1,na).*(DA(1:2:end-1,:)+1i*DA(2:2:end,:));
    dug(1:2:end,:) = real(cdg)-dug(1:2:end,:);
    dug(2:2:end,:) = imag(cdg)-dug(2:2:end,:);

    jac = [dug dtg ddg];
    
%     ddug = eye(na);
%     cdg = repmat(k,1,na).*cdg;
%     ddug(1:2:end,:) = real(cdg);  ddug(2:2:end,:) = imag(cdg);
%     
%     dutg = DF*DA;
%     cdg = repmat(ek,1,na).*(dutg(1:2:end-1,:)+1i*dutg(2:2:end,:));
%     dutg(1:2:end,:) = real(cdg);  dutg(2:2:end,:) = imag(cdg);
%     
%     dt2g = DF*FT;
%     cf = ek.*(dt2g(1:2:end-1,:)+1i*dt2g(2:2:end,:));
%     dt2g(1:2:end) = real(cf);  dt2g(2:2:end) = imag(cf);
%     
%     cf = k.*cf;  ddtg = zeros(na,1);
%     ddtg(1:2:end) = real(cf);  ddtg(2:2:end) = imag(cf);
%     
%     cg = k.*cg;  dd2g = zeros(na,1);
%     dd2g(1:2:end) = real(cg);  dd2g(2:2:end) = imag(cg);
% 
%     jac = [C; -alpha(1).*dtg'; -alpha(2).*ddg']*[dug dtg ddg];
%     jac(na+1,:) = jac(na+1,:) - alpha(1).*g'*[dutg dt2g ddtg];
%     jac(na+2,:) = jac(na+2,:) - alpha(2).*g'*[ddug ddtg dd2g];  
  end
  
%  g = [C; -alpha(1).*dtg'; -alpha(2).*ddg']*g; 
return;
