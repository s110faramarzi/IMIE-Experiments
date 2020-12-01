
%{
   A code to extract phase of oscillations from caclium imaging data using
   the SWIFT algorithm, https://ieeexplore.ieee.org/document/8026592
   Written by Sadegh Faramarzi
%}

myData=(sum(filtData))'; %time series of calcium data
N= length(myData);
CF = 6.67; %central frequency
SR = 100; %smapling rate
w = 2*pi*CF/SR;  % window function
tau = 4*SR/CF; % time constant of the wondow
D = exp(-1/tau)*exp(1j*w);
Xjw = zeros(N,1);
Xjw(1)=0;%myData(1);
for i=2:N
    Xjw(i) = D*Xjw(i-1)+myData(i)-myData(i-1);
end

% Plot the data
% N=length
figure;

ax10=subplot(2,2,1);  % create a plot with subplots in a grid of 4 x 2

plot([0:N-1]*0.01, detrend(COEFF(1:tt,7)));
set(gca,'XTickLabel',[]);
set(gca,'YTickLabel',[]);
ax1=subplot(2,2,3);
plot([0:N-1]*0.01, real(Xjw), [0:N-1]*0.01, imag(Xjw), [0:N-1]*0.01, abs(Xjw))
% legend({'Real','Imaginary','Abs'});
legend({'Real','Imaginary','Abs'},'Orientation','horizontal');

set(gca,'YTickLabel',[]); 
linkaxes([ax10 ax1],'x');
legend boxoff;
% subplot(3,1,2)
ax2=subplot(2,2,[2 4]);

[a,b] = pmtm(COEFF(:,7),2,length(COEFF(:,i)),Fs,'unity');
plot(b+.15,pow2db(a)+165,'b','linewidth',2);hold on;xlim([0,20]);
[a,b] = pmtm(real(Xjw),2,length(real(Xjw)),Fs,'unity');
plot(b,pow2db(a),'k','linewidth',2); 
xlim([0,20]);


legend('Raw data','X_{6.7Hz}');

legend boxoff  
a=findobj(gcf); % get the handles associated with the current figure

allaxes=findall(a,'Type','axes');
alllines=findall(a,'Type','line');
alltext=findall(a,'Type','text');
% 
set(allaxes,'FontName','Arial','LineWidth',2,...
    'FontSize',14);
set(alllines,'Linewidth',2);
set(alltext,'interpreter','LaTex','FontName','Arial','FontWeight','Bold','FontSize',14);
box on;

%%


figure;

ax1=subplot(2,2,[1 3]);  % create a plot with subplots in a grid of 4 x 2
plot([0:N-1]*0.01,angle(Xjw));
hold on; plot([0:N-1]*0.01,stims); legend('Phase','Stim.');
set(gca,'YTickLabel',[]); legend boxoff;
% legend('Raw data','X_{6.7Hz}');

legend boxoff;
% subplot(3,1,2)
ax2=subplot(2,2,[2 4]);
scatter( prcmtrx(2,:)/pi, prcmtrx(6,:)*2);
xlim([0,2.1]);
ylim([-1.5,0.5]);
xticklabels({'0','\pi/2','\pi','3\pi/4','2\pi', 'FontName','Arial','FontWeight','Bold','LineWidth',2,...
    'FontSize',14});
yticklabels({'-1.5\pi','-\pi','-\pi/2','0','0.5\pi', 'latex','FontWeight','Bold','LineWidth',2,...
    'FontSize',14});


ax = gca;
ax.FontSize = 12;
grid off;




% 
% ax3=subplot(4,2,[5 7]);
% % subplot(3,1,3)
% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% pwelch
% % [a,b]=pwelch(real(Xjw),256,128,256,100);
% % % [a,b]=pwelch(y,length(y),length(y)/2,length(y),Fs);
% % plot(b,pow2db(a),'k','linewidth',3);
% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  pmtm
% % [a,b] = pmtm(COEFF(:,7),2,length(COEFF(:,i)),Fs,'unity');
% % plot(b+.15,pow2db(a)+156.9,'b','linewidth',3);hold on;
% % [a,b] = pmtm(real(Xjw),2,length(real(Xjw)),Fs,'unity');
% % plot(b,pow2db(a),'k','linewidth',3); 
% 
% plot([0:N-1]*0.01,angle(Xjw));
% hold on; plot([0:N-1]*0.01,stims); legend('Phase','Stim.');
% set(gca,'YTickLabel',[]); legend boxoff;
% % legend('Raw data','X_{6.7Hz}');
% 
% legend boxoff  
% % set(h,'Interpreter','latex');
% % legend('Raw data','X_6 Hz');



% pmtm(real(Xjw),3,length(real(Xjw)),Fs,'unity')


% 
% 
% ax4=subplot(4,2,[ 6 8]);
% scatter( prcmtrx(2,:)/pi, prcmtrx(6,:)*2);
% xlim([0,2.1]);
% ylim([-1.5,0.5]);
% xticklabels({'0','\pi/2','\pi','3\pi/4','2\pi', 'FontName','Arial','FontWeight','Bold','LineWidth',2,...
%     'FontSize',14});
% yticklabels({'-1.5\pi','-\pi','-\pi/2','0','0.5\pi', 'latex','FontWeight','Bold','LineWidth',2,...
%     'FontSize',14});
% 
% linkaxes([ax10 ax1 ax2],'x');
% ax = gca;
% ax.FontSize = 12;
% grid off;
% 
a=findobj(gcf); % get the handles associated with the current figure

allaxes=findall(a,'Type','axes');
alllines=findall(a,'Type','line');
alltext=findall(a,'Type','text');
% 
set(allaxes,'FontName','Arial','LineWidth',2,...
    'FontSize',14);
set(alllines,'Linewidth',2);
set(alltext,'interpreter','LaTex','FontName','Arial','FontWeight','Bold','FontSize',14);
box on;

% prcfit=-0.1*(exp(prcmtrx(2,:)-pi))./(exp(prcmtrx(2,:)-pi)+1)+0.05;
% scatter( prcmtrx(2,:),prcfit)


% 
% ax1=subplot(2,1,1);
% plot(phase1,'b');hold on;
% plot(phase2,'r');
% plot(phase3,'m');
% plot(phase4,'k');hold on;
% ax2=subplot(2,1,2);
% plot(stims);
% 
% % plot(stims,'k');
% linkaxes([ax1 ax2],'x');
